#!/usr/bin/perl
#*****************************************************************************
#* vlc-api.pl: VLC API maintenance script
#*****************************************************************************
#* Copyright (C) 2005 the VideoLAN team
#* $Id$
#*
#* Authors: Rémi Denis-Courmont <rem # videolan.org>
#*
#* This program is free software; you can redistribute it and/or modify
#* it under the terms of the GNU General Public License as published by
#* the Free Software Foundation; either version 2 of the License, or
#* (at your option) any later version.
#*
#* This program is distributed in the hope that it will be useful,
#* but WITHOUT ANY WARRANTY; without even the implied warranty of
#* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#* GNU General Public License for more details.
#*
#* You should have received a copy of the GNU General Public License
#* along with this program; if not, write to the Free Software
#* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston MA 02110-1301, USA.
#*****************************************************************************/

use IO::Handle;
use strict;

my $srcdir = $ENV{'top_srcdir'};

#
# Reads to-be exported APIs
#
my %new_APIs;

while (<STDIN>)
{
	if (/VLC_EXPORT\(\s*(\w.*\S)\s*,\s*(\w*)\s*,\s*\(\s*(\w.*\S)\s*\)\s*\)[^)]*$/)
	{
		$new_APIs{$2} = [ ( $1, $3 ) ];
	} 
}

#
# Write header's header
#
my $new_sym=IO::Handle->new();
open $new_sym, '> vlc_symbols.h.new' or die "$!";
print { $new_sym }
	"/*\n".
	" * This file is automatically generated. DO NOT EDIT!\n".
	" * You can force an update with \"make stamp-api\".\n".
	" */\n".
	"\n".
	"#ifndef __VLC_SYMBOLS_H\n".
	"# define __VLC_SYMBOLS_H\n".
	"\n".
	"# ifdef HAVE_SHARED_LIBVLC\n".
	"#  error You are not supposed to include this file!\n".
	"# endif\n".
	"/*\n".
	" * This is the big VLC API structure for plugins :\n".
	" * Changing its layout breaks plugin's binary compatibility,\n".
	" * so DO NOT DO THAT.\n".
	" * In case of conflict with SVN, add your uncommited APIs\n".
	" * at the *end* of the structure so they don't mess the other's\n".
	" * offset in the structure.\n".
	" */\n".
	"struct module_symbols_t\n".
	"{\n";

my $changes = 0;

#
# Compares new APIs with currently exported APIs
#
my @API;
my @deprecated_API;
my $parse = 0;

my $oldfd = IO::Handle->new();
open $oldfd, "< $srcdir/include/vlc_symbols.h";

while (<$oldfd>)
{
	if (/^struct module_symbols_t/)
	{
		$parse = 1;
	}
	elsif ($parse == 0)
	{
	}
	elsif (/^    void \*(\w*)_deprecated;$/)
	{
		if (defined $new_APIs{$1})
		{
			print "[info] $1 was RESTORED!\n";
			print { $new_sym }
				"    ".$new_APIs{$1}[0]." (*$1_inner) (".$new_APIs{$1}[1].");\n";
			delete $new_APIs{$1};
			push (@API, $1);
			$changes++;
		}
		else
		{
			print { $new_sym } $_;
			push (@deprecated_API, $1);
		}
	}
	elsif (/^\s*(\w.*\S)\s*\(\*\s*(\w*)_inner\)\s*\(\s*(\w.*\S)\s*\)\s*;\s*$/)
	{
		if (!defined $new_APIs{$2})
		{
			print "[warn] $2 was REMOVED!\n";
			print { $new_sym } "    void *$2_deprecated;\n";
			push (@deprecated_API, $2);
			$changes++;
		}
		elsif (($new_APIs{$2}[0] ne $1)
		    || ($new_APIs{$2}[1] ne $3))
		{
			print
				"[warn] $2 was CHANGED!\n".
				"       Old argument(s) : \"$3\"\n".
				"       New argument(s) : \"".$new_APIs{$2}[1]."\"\n".
				"       Old return type : \"$1\"\n".
				"       New return type : \"".$new_APIs{$2}[0]."\"\n";

			print { $new_sym }
				"    ".$new_APIs{$2}[0]." (*$2_inner) (".$new_APIs{$2}[1].");\n";
			delete $new_APIs{$2};
			push (@API, $2);
			$changes++;
		}
		else
		{
			print { $new_sym } "    $1 (*$2_inner) ($3);\n";
			push (@API, $2);
			delete $new_APIs{$2};
		}
	}
}
close $oldfd;

#
# Adds brand-new APIs
#
foreach (keys %new_APIs)
{
	print "[info] $_ was ADDED!\n";
	print { $new_sym }
		"    ".$new_APIs{$_}[0]." (*${_}_inner) (".$new_APIs{$_}[1].");\n";
	push (@API, $_);
	$changes++;
}

#
# Writes #defines
#
print { $new_sym }
	"};\n".
	"# if defined (__PLUGIN__)\n";

foreach (@API)
{
	print { $new_sym } "#  define $_ (p_symbols)->${_}_inner\n";
}

print { $new_sym }
	"# elif defined (HAVE_DYNAMIC_PLUGINS) && !defined (__BUILTIN__)\n".
	"/******************************************************************\n".
	" * STORE_SYMBOLS: store VLC APIs into p_symbols for plugin access.\n".
	" ******************************************************************/\n".
	"#  define STORE_SYMBOLS( p_symbols ) \\\n";

foreach (@API)
{
	print { $new_sym } "    ((p_symbols)->${_}_inner) = $_; \\\n";
}
foreach (@deprecated_API)
{
	print { $new_sym } "    (p_symbols)->${_}_deprecated = NULL; \\\n";
}

print { $new_sym }
	"\n".
	"# endif /* __PLUGIN__ */\n".
	"#endif /* __VLC_SYMBOLS_H */\n";
close $new_sym;

#
# Replace headers if needed
#
if ($changes != 0)
{
	rename 'vlc_symbols.h.new', "$srcdir/include/vlc_symbols.h";
	print "$changes API(s) changed.\n";
}
else
{
	unlink 'vlc_symbols.h.new';
}
