/* File generated by GAA 1.6.6
 */
#define GAA_NO_WIN32
#line 1 "cli.gaa"


/* C declarations */

#include <config.h>
#include <common.h>
#include <benchmark.h>

void cli_version(void);

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef GAA_NO_WIN32
#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(WINDOWS)
#define GAA_WIN32
#endif
#endif

static void* gaa_malloc( size_t size) {
void* ret;
	ret = malloc(size);
	if (ret==NULL) {
		fprintf(stderr, "gaa: could not allocate memory");
		exit(1);
	}
	return ret;
}

static void __gaa_helpsingle(char short_name, char *name, 
	char *arg_desc, char *opt_help)
{
     int col1, col3, col4, tabsize = 3, curr;
     int i;

     col1 = 5; /* Default values */
     col3 = 30;
     col4 = 70;

     curr = 0;
     for(i = 0; i < col1; i++)
        {
        printf(" ");
        curr++;
        }
     if(short_name)
        {
        if(name && *name)
          {
            printf("-%c, ", short_name);
            curr += 4;
          }
        else
          {
            printf("-%c ", short_name);
            curr += 3;
          }
        }
     if(name && *name)
        {
          printf("--%s ", name);
          curr += 3 + strlen(name);
        }
     if(arg_desc && *arg_desc)
        {
          printf("%s ", arg_desc);
          curr += 1 + strlen(arg_desc);
        }
     if(curr >= col3)
        {
          printf("\n");
          curr = 0;
        }
     if(opt_help) /* let's print the option's help body */
        {
        const char *str = opt_help;
        while(*str)
          {
             while(curr < col3)
               {
                 printf(" ");
                 curr++;
               }
             switch(*str)
               {
                 case '\n':
                     printf("\n");
                     curr = 0;
                     break;
                 case '\t':
                     do
                        {
                        printf(" ");
                        curr++;
                        }
                     while((curr - col3) % tabsize != 0 && curr < col4);
                 case ' ':
                     if(*str == ' ')
                        {
                        curr++;
                        printf(" ");
                        }
                     for(i = 1; str[i] && str[i] != ' ' && str[i] != '\n'
                        && str[i] != '\t'; i++);
                     if(curr + i - 1 >= col4)
                        curr = col4;
                     break;
                default:
                     printf("%c", *str);
                     curr++;
               }
             if(curr >= col4)
               {
                 printf("\n");
                 curr = 0;
               }
             str++;
          }
        }
     printf("\n");
}

void gaa_help(void)
{
	printf("GnuTLS test client\nUsage:  gnutls-cli [options] hostname\n\n\n");
	__gaa_helpsingle('d', "debug", "integer ", "Enable debugging");
	__gaa_helpsingle('r', "resume", "", "Connect, establish a session. Connect again and resume this session.");
	__gaa_helpsingle('e', "rehandshake", "", "Connect, establish a session and rehandshake immediately.");
	__gaa_helpsingle(0, "noticket", "", "Doesn't accept session tickets.");
	__gaa_helpsingle('s', "starttls", "", "Connect, establish a plain session and start TLS when EOF or a SIGALRM is received.");
	__gaa_helpsingle('u', "udp", "", "Use DTLS (datagram TLS) over UDP.");
	__gaa_helpsingle(0, "mtu", "integer ", "Set MTU for datagram TLS.");
	__gaa_helpsingle(0, "crlf", "", "Send CR LF instead of LF.");
	__gaa_helpsingle(0, "x509fmtder", "", "Use DER format for certificates to read from.");
	__gaa_helpsingle('f', "fingerprint", "", "Send the openpgp fingerprint, instead of the key.");
	__gaa_helpsingle(0, "disable-extensions", "", "Disable all the TLS extensions.");
	__gaa_helpsingle(0, "print-cert", "", "Print the certificate in PEM format.");
	__gaa_helpsingle(0, "recordsize", "integer ", "The maximum record size to advertize.");
	__gaa_helpsingle('V', "verbose", "", "More verbose output.");
	__gaa_helpsingle(0, "priority", "PRIORITY STRING ", "Priorities string.");
	__gaa_helpsingle(0, "x509cafile", "FILE ", "Certificate file or PKCS #11 URL to use.");
	__gaa_helpsingle(0, "x509crlfile", "FILE ", "CRL file to use.");
	__gaa_helpsingle(0, "pgpkeyfile", "FILE ", "PGP Key file to use.");
	__gaa_helpsingle(0, "pgpkeyring", "FILE ", "PGP Key ring file to use.");
	__gaa_helpsingle(0, "pgpcertfile", "FILE ", "PGP Public Key (certificate) file to use.");
	__gaa_helpsingle(0, "pgpsubkey", "HEX|auto ", "PGP subkey to use.");
	__gaa_helpsingle(0, "x509keyfile", "FILE ", "X.509 key file or PKCS #11 URL to use.");
	__gaa_helpsingle(0, "x509certfile", "FILE ", "X.509 Certificate file or PKCS #11 URL to use.");
	__gaa_helpsingle(0, "srpusername", "NAME ", "SRP username to use.");
	__gaa_helpsingle(0, "srppasswd", "PASSWD ", "SRP password to use.");
	__gaa_helpsingle(0, "pskusername", "NAME ", "PSK username to use.");
	__gaa_helpsingle(0, "pskkey", "KEY ", "PSK key (in hex) to use.");
	__gaa_helpsingle('p', "port", "PORT ", "The port to connect to.");
	__gaa_helpsingle(0, "insecure", "", "Don't abort program if server certificate can't be validated.");
	__gaa_helpsingle(0, "benchmark-ciphers", "", "Benchmark individual ciphers.");
	__gaa_helpsingle(0, "benchmark-tls", "", "Benchmark ciphers and key exchange methods in TLS.");
	__gaa_helpsingle('l', "list", "", "Print a list of the supported algorithms and modes.");
	__gaa_helpsingle('h', "help", "", "prints this help");
	__gaa_helpsingle('v', "version", "", "prints the program's version number");

#line 100 "gaa.skel"
}
/* Copy of C area */

#line 104 "gaa.skel"
/* GAA HEADER */
#ifndef GAA_HEADER_POKY
#define GAA_HEADER_POKY

typedef struct _gaainfo gaainfo;

struct _gaainfo
{
#line 110 "cli.gaa"
	char *rest_args;
#line 99 "cli.gaa"
	int insecure;
#line 96 "cli.gaa"
	char *port;
#line 93 "cli.gaa"
	char *psk_key;
#line 90 "cli.gaa"
	char *psk_username;
#line 87 "cli.gaa"
	char *srp_passwd;
#line 84 "cli.gaa"
	char *srp_username;
#line 81 "cli.gaa"
	char *x509_certfile;
#line 78 "cli.gaa"
	char *x509_keyfile;
#line 75 "cli.gaa"
	char *pgp_subkey;
#line 72 "cli.gaa"
	char *pgp_certfile;
#line 69 "cli.gaa"
	char *pgp_keyring;
#line 66 "cli.gaa"
	char *pgp_keyfile;
#line 63 "cli.gaa"
	char *x509_crlfile;
#line 60 "cli.gaa"
	char *x509_cafile;
#line 57 "cli.gaa"
	char *priorities;
#line 54 "cli.gaa"
	int verbose;
#line 51 "cli.gaa"
	int record_size;
#line 48 "cli.gaa"
	int print_cert;
#line 45 "cli.gaa"
	int disable_extensions;
#line 42 "cli.gaa"
	int fingerprint;
#line 39 "cli.gaa"
	int fmtder;
#line 36 "cli.gaa"
	int crlf;
#line 33 "cli.gaa"
	int mtu;
#line 30 "cli.gaa"
	int udp;
#line 27 "cli.gaa"
	int starttls;
#line 24 "cli.gaa"
	int noticket;
#line 21 "cli.gaa"
	int rehandshake;
#line 18 "cli.gaa"
	int resume;
#line 15 "cli.gaa"
	int debug;

#line 114 "gaa.skel"
};

#ifdef __cplusplus
extern "C"
{
#endif

    int gaa(int argc, char *argv[], gaainfo *gaaval);

    void gaa_help(void);
    
    int gaa_file(const char *name, gaainfo *gaaval);
    
#ifdef __cplusplus
}
#endif


#endif

#line 135 "gaa.skel"

/* C declarations */

#define GAAERROR(x)    \
{                   \
gaa_error = 1;      \
return x;        \
}

static char *gaa_current_option;
static int gaa_error = 0;

/* Generated by gaa */

#include <string.h>
#include <stdlib.h>


#define GAA_OK                       -1

#define GAA_ERROR_NOMATCH            0
#define GAA_ERROR_NOTENOUGH_ARGS     1
#define GAA_ERROR_INVALID_ARG        2
#define GAA_ERROR_UNKNOWN            3

#define GAA_NOT_AN_OPTION       0
#define GAA_WORD_OPTION         1
#define GAA_LETTER_OPTION       2
#define GAA_MULTIPLE_OPTION     3

#define GAA_REST                0
#define GAA_NB_OPTION           34
#define GAAOPTID_version	1
#define GAAOPTID_help	2
#define GAAOPTID_list	3
#define GAAOPTID_benchmark_tls	4
#define GAAOPTID_benchmark_ciphers	5
#define GAAOPTID_insecure	6
#define GAAOPTID_port	7
#define GAAOPTID_pskkey	8
#define GAAOPTID_pskusername	9
#define GAAOPTID_srppasswd	10
#define GAAOPTID_srpusername	11
#define GAAOPTID_x509certfile	12
#define GAAOPTID_x509keyfile	13
#define GAAOPTID_pgpsubkey	14
#define GAAOPTID_pgpcertfile	15
#define GAAOPTID_pgpkeyring	16
#define GAAOPTID_pgpkeyfile	17
#define GAAOPTID_x509crlfile	18
#define GAAOPTID_x509cafile	19
#define GAAOPTID_priority	20
#define GAAOPTID_verbose	21
#define GAAOPTID_recordsize	22
#define GAAOPTID_print_cert	23
#define GAAOPTID_disable_extensions	24
#define GAAOPTID_fingerprint	25
#define GAAOPTID_x509fmtder	26
#define GAAOPTID_crlf	27
#define GAAOPTID_mtu	28
#define GAAOPTID_udp	29
#define GAAOPTID_starttls	30
#define GAAOPTID_noticket	31
#define GAAOPTID_rehandshake	32
#define GAAOPTID_resume	33
#define GAAOPTID_debug	34

#line 168 "gaa.skel"

#define GAA_CHECK1STR(a,b)      \
if(a[0] == str[0])              \
{                               \
    gaa_current_option = a;     \
    return b;                   \
}

#define GAA_CHECKSTR(a,b)                \
if(strcmp(a,str) == 0)                   \
{                                        \
    gaa_current_option = a;              \
    return b;                            \
}

#define GAA_TESTMOREARGS                                                  \
if(!OK)                                                                     \
{                                  \
while((gaa_last_non_option != gaa_index) && (gaa_arg_used[gaa_index] == 1)) \
    gaa_index++;                                                            \
if(gaa_last_non_option == gaa_index)                                        \
    return GAA_ERROR_NOTENOUGH_ARGS; \
}

#define GAA_TESTMOREOPTIONALARGS                                                  \
if(!OK) \
{ \
while((gaa_last_non_option != gaa_index) && (gaa_arg_used[gaa_index] == 1)) \
    gaa_index++;                                                            \
if(gaa_last_non_option == gaa_index)                                        \
    OK = 1; \
}

#define GAA_FILL_2ARGS(target, func)           \
target = func(GAAargv[gaa_index]);       \
gaa_arg_used[gaa_index] = 1;             \
if(gaa_error == 1)                       \
{                                        \
    gaa_error = 0;                       \
    return GAA_ERROR_INVALID_ARG;        \
} 



#define GAA_FILL(target, func, num)           \
if(!OK) \
{ \
target = func(GAAargv[gaa_index]);       \
gaa_arg_used[gaa_index] = 1;             \
if(gaa_error == 1)                       \
{                                        \
    gaa_error = 0;                       \
    return GAA_ERROR_INVALID_ARG;        \
} \
num = 1;  \
} \
else \
{ \
num = 0; \
}

#define GAA_LIST_FILL(target, func, type ,num)                      \
if(!OK) \
{ \
num = 0;                                                            \
target = NULL;                                                      \
if ( gaa_last_non_option - gaa_index > 0)                           \
  target = gaa_malloc((gaa_last_non_option - gaa_index) * sizeof(type));\
for(; gaa_index < gaa_last_non_option; gaa_index++)                 \
{                                                                   \
    if(gaa_arg_used[gaa_index] == 0)                                \
    {                                                               \
        GAA_FILL_2ARGS(target[num], func);                          \
        num++;                                                      \
    }                                                               \
}                                                                   \
if(num == 0)                                                        \
    return GAA_ERROR_NOTENOUGH_ARGS; \
}

#define GAA_OPTIONALLIST_FILL(target, func, type ,num)                      \
if(!OK) \
{ \
num = 0;                                                            \
target = NULL;                                                      \
if ( gaa_last_non_option - gaa_index > 0)                           \
  target = gaa_malloc((gaa_last_non_option - gaa_index) * sizeof(type));\
for(; gaa_index < gaa_last_non_option; gaa_index++)                 \
{                                                                   \
    if(gaa_arg_used[gaa_index] == 0)                                \
    {                                                               \
        GAA_FILL_2ARGS(target[num], func);                                \
        num++;                                                      \
    }                                                               \
} \
}

#define GAA_OBLIGAT(str)                                            \
k = 0;                                                              \
for(i = 0; i < strlen(str); i++)                                    \
{                                                                   \
    j = gaa_get_option_num(str + i, GAA_LETTER_OPTION);           \
    if(j == GAA_ERROR_NOMATCH)                                       \
    {                                                               \
        printf("Error: invalid 'obligat' set\n");                  \
        exit(-1);                                                   \
    }                                                               \
    if(opt_list[j] == 1)                                            \
        k = 1;                                                      \
}                                                                    \
if(k == 0)                                                            \
{                                                                      \
    if(strlen(str) == 1)                                                \
        printf("You must give the -%s option\n", str);                     \
    else                                                                  \
        printf("You must give at least one option of '%s'\n", str);          \
    return 0;         \
}
        
#define GAA_INCOMP(str)                                                \
k = 0;                                                              \
for(i = 0; i < strlen(str); i++)                                    \
{                                                                   \
    j = gaa_get_option_num(str + i, GAA_LETTER_OPTION);           \
    if(j == GAA_ERROR_NOMATCH)                                      \
    {                                                               \
        printf("Error: invalid 'obligat' set\n");                  \
        exit(-1);                                                   \
    }                                                               \
    if(opt_list[j] == 1)                                            \
        k++;                                                      \
}                   \
if(k > 1)                                                            \
{                                                                      \
    printf("The options '%s' are incompatible\n", str);              \
    return 0;                                                          \
}
        

static char **GAAargv;
static int GAAargc;
static char *gaa_arg_used;
static int gaa_processing_file = 0;
static int inited = 0;

static int gaa_getint(char *arg)
{
    int tmp;
    char a;
    if(sscanf(arg, "%d%c", &tmp, &a) < 1)
    {
        printf("Option %s: '%s' isn't an integer\n", gaa_current_option, arg);
        GAAERROR(-1);
    }
    return tmp;
}

static char gaa_getchar(char *arg)
{
    if(strlen(arg) != 1)
    {
        printf("Option %s: '%s' isn't an character\n", gaa_current_option, arg);
        GAAERROR(-1);
    }
    return arg[0];
}

static char* gaa_getstr(char *arg)
{
    return arg;
}
static float gaa_getfloat(char *arg)
{
    float tmp;
    char a;
    if(sscanf(arg, "%f%c", &tmp, &a) < 1)
    {
        printf("Option %s: '%s' isn't a float number\n", gaa_current_option, arg);
        GAAERROR(-1);
    }
    return tmp;
}
/* option structures */

struct GAAOPTION_port 
{
	char* arg1;
	int size1;
};

struct GAAOPTION_pskkey 
{
	char* arg1;
	int size1;
};

struct GAAOPTION_pskusername 
{
	char* arg1;
	int size1;
};

struct GAAOPTION_srppasswd 
{
	char* arg1;
	int size1;
};

struct GAAOPTION_srpusername 
{
	char* arg1;
	int size1;
};

struct GAAOPTION_x509certfile 
{
	char* arg1;
	int size1;
};

struct GAAOPTION_x509keyfile 
{
	char* arg1;
	int size1;
};

struct GAAOPTION_pgpsubkey 
{
	char* arg1;
	int size1;
};

struct GAAOPTION_pgpcertfile 
{
	char* arg1;
	int size1;
};

struct GAAOPTION_pgpkeyring 
{
	char* arg1;
	int size1;
};

struct GAAOPTION_pgpkeyfile 
{
	char* arg1;
	int size1;
};

struct GAAOPTION_x509crlfile 
{
	char* arg1;
	int size1;
};

struct GAAOPTION_x509cafile 
{
	char* arg1;
	int size1;
};

struct GAAOPTION_priority 
{
	char* arg1;
	int size1;
};

struct GAAOPTION_recordsize 
{
	int arg1;
	int size1;
};

struct GAAOPTION_mtu 
{
	int arg1;
	int size1;
};

struct GAAOPTION_debug 
{
	int arg1;
	int size1;
};
#define GAA_REST_EXISTS

struct GAAREST
{
	char* arg1;
	int size1;
};
         
#line 349 "gaa.skel"
static int gaa_is_an_argument(char *str)
{
#ifdef GAA_WIN32
    if(str[0] == '/' && str[1] != 0)
	return GAA_MULTIPLE_OPTION;
#endif
    if(str[0] != '-')
        return GAA_NOT_AN_OPTION;
    if(str[1] == 0)
        return GAA_NOT_AN_OPTION;
    if(str[1] == '-')
    {
        if(str[2] != 0)
            return GAA_WORD_OPTION;
        else
            return GAA_NOT_AN_OPTION;
    }
    if(str[2] == 0)
        return GAA_LETTER_OPTION;
    else
        return GAA_MULTIPLE_OPTION;
}

static int gaa_get_option_num(char *str, int status)
{
    switch(status)
        {
        case GAA_LETTER_OPTION:
			GAA_CHECK1STR("p", GAAOPTID_port);
			GAA_CHECK1STR("", GAAOPTID_pskkey);
			GAA_CHECK1STR("", GAAOPTID_pskusername);
			GAA_CHECK1STR("", GAAOPTID_srppasswd);
			GAA_CHECK1STR("", GAAOPTID_srpusername);
			GAA_CHECK1STR("", GAAOPTID_x509certfile);
			GAA_CHECK1STR("", GAAOPTID_x509keyfile);
			GAA_CHECK1STR("", GAAOPTID_pgpsubkey);
			GAA_CHECK1STR("", GAAOPTID_pgpcertfile);
			GAA_CHECK1STR("", GAAOPTID_pgpkeyring);
			GAA_CHECK1STR("", GAAOPTID_pgpkeyfile);
			GAA_CHECK1STR("", GAAOPTID_x509crlfile);
			GAA_CHECK1STR("", GAAOPTID_x509cafile);
			GAA_CHECK1STR("", GAAOPTID_priority);
			GAA_CHECK1STR("", GAAOPTID_recordsize);
			GAA_CHECK1STR("", GAAOPTID_mtu);
			GAA_CHECK1STR("d", GAAOPTID_debug);
        case GAA_MULTIPLE_OPTION:
#line 375 "gaa.skel"
			GAA_CHECK1STR("v", GAAOPTID_version);
			GAA_CHECK1STR("h", GAAOPTID_help);
			GAA_CHECK1STR("l", GAAOPTID_list);
			GAA_CHECK1STR("", GAAOPTID_benchmark_tls);
			GAA_CHECK1STR("", GAAOPTID_benchmark_ciphers);
			GAA_CHECK1STR("", GAAOPTID_insecure);
			GAA_CHECK1STR("V", GAAOPTID_verbose);
			GAA_CHECK1STR("", GAAOPTID_print_cert);
			GAA_CHECK1STR("", GAAOPTID_disable_extensions);
			GAA_CHECK1STR("f", GAAOPTID_fingerprint);
			GAA_CHECK1STR("", GAAOPTID_x509fmtder);
			GAA_CHECK1STR("", GAAOPTID_crlf);
			GAA_CHECK1STR("u", GAAOPTID_udp);
			GAA_CHECK1STR("s", GAAOPTID_starttls);
			GAA_CHECK1STR("", GAAOPTID_noticket);
			GAA_CHECK1STR("e", GAAOPTID_rehandshake);
			GAA_CHECK1STR("r", GAAOPTID_resume);

#line 277 "gaa.skel"
        break;
        case GAA_WORD_OPTION:
			GAA_CHECKSTR("version", GAAOPTID_version);
			GAA_CHECKSTR("help", GAAOPTID_help);
			GAA_CHECKSTR("list", GAAOPTID_list);
			GAA_CHECKSTR("benchmark-tls", GAAOPTID_benchmark_tls);
			GAA_CHECKSTR("benchmark-ciphers", GAAOPTID_benchmark_ciphers);
			GAA_CHECKSTR("insecure", GAAOPTID_insecure);
			GAA_CHECKSTR("port", GAAOPTID_port);
			GAA_CHECKSTR("pskkey", GAAOPTID_pskkey);
			GAA_CHECKSTR("pskusername", GAAOPTID_pskusername);
			GAA_CHECKSTR("srppasswd", GAAOPTID_srppasswd);
			GAA_CHECKSTR("srpusername", GAAOPTID_srpusername);
			GAA_CHECKSTR("x509certfile", GAAOPTID_x509certfile);
			GAA_CHECKSTR("x509keyfile", GAAOPTID_x509keyfile);
			GAA_CHECKSTR("pgpsubkey", GAAOPTID_pgpsubkey);
			GAA_CHECKSTR("pgpcertfile", GAAOPTID_pgpcertfile);
			GAA_CHECKSTR("pgpkeyring", GAAOPTID_pgpkeyring);
			GAA_CHECKSTR("pgpkeyfile", GAAOPTID_pgpkeyfile);
			GAA_CHECKSTR("x509crlfile", GAAOPTID_x509crlfile);
			GAA_CHECKSTR("x509cafile", GAAOPTID_x509cafile);
			GAA_CHECKSTR("priority", GAAOPTID_priority);
			GAA_CHECKSTR("verbose", GAAOPTID_verbose);
			GAA_CHECKSTR("recordsize", GAAOPTID_recordsize);
			GAA_CHECKSTR("print-cert", GAAOPTID_print_cert);
			GAA_CHECKSTR("disable-extensions", GAAOPTID_disable_extensions);
			GAA_CHECKSTR("fingerprint", GAAOPTID_fingerprint);
			GAA_CHECKSTR("x509fmtder", GAAOPTID_x509fmtder);
			GAA_CHECKSTR("crlf", GAAOPTID_crlf);
			GAA_CHECKSTR("mtu", GAAOPTID_mtu);
			GAA_CHECKSTR("udp", GAAOPTID_udp);
			GAA_CHECKSTR("starttls", GAAOPTID_starttls);
			GAA_CHECKSTR("noticket", GAAOPTID_noticket);
			GAA_CHECKSTR("rehandshake", GAAOPTID_rehandshake);
			GAA_CHECKSTR("resume", GAAOPTID_resume);
			GAA_CHECKSTR("debug", GAAOPTID_debug);

#line 281 "gaa.skel"
	break;
        default: break;
        }
    return GAA_ERROR_NOMATCH;
}

static int gaa_try(int gaa_num, int gaa_index, gaainfo *gaaval, char *opt_list)
{
    int OK = 0;
    int gaa_last_non_option;
	struct GAAOPTION_port GAATMP_port;
	struct GAAOPTION_pskkey GAATMP_pskkey;
	struct GAAOPTION_pskusername GAATMP_pskusername;
	struct GAAOPTION_srppasswd GAATMP_srppasswd;
	struct GAAOPTION_srpusername GAATMP_srpusername;
	struct GAAOPTION_x509certfile GAATMP_x509certfile;
	struct GAAOPTION_x509keyfile GAATMP_x509keyfile;
	struct GAAOPTION_pgpsubkey GAATMP_pgpsubkey;
	struct GAAOPTION_pgpcertfile GAATMP_pgpcertfile;
	struct GAAOPTION_pgpkeyring GAATMP_pgpkeyring;
	struct GAAOPTION_pgpkeyfile GAATMP_pgpkeyfile;
	struct GAAOPTION_x509crlfile GAATMP_x509crlfile;
	struct GAAOPTION_x509cafile GAATMP_x509cafile;
	struct GAAOPTION_priority GAATMP_priority;
	struct GAAOPTION_recordsize GAATMP_recordsize;
	struct GAAOPTION_mtu GAATMP_mtu;
	struct GAAOPTION_debug GAATMP_debug;

#line 393 "gaa.skel"
#ifdef GAA_REST_EXISTS
    struct GAAREST GAAREST_tmp;
#endif

    opt_list[gaa_num] = 1;
    
    for(gaa_last_non_option = gaa_index;
        (gaa_last_non_option != GAAargc) && (gaa_is_an_argument(GAAargv[gaa_last_non_option]) == GAA_NOT_AN_OPTION);
        gaa_last_non_option++);

    if(gaa_num == GAA_REST)
    {
        gaa_index = 1;
        gaa_last_non_option = GAAargc;
    }
    
    switch(gaa_num)
    {
	case GAAOPTID_version:
	OK = 0;
#line 108 "cli.gaa"
{ cli_version(); exit(0); ;};

		return GAA_OK;
		break;
	case GAAOPTID_help:
	OK = 0;
#line 106 "cli.gaa"
{ gaa_help(); exit(0); ;};

		return GAA_OK;
		break;
	case GAAOPTID_list:
	OK = 0;
#line 105 "cli.gaa"
{ print_list(gaaval->verbose); exit(0); ;};

		return GAA_OK;
		break;
	case GAAOPTID_benchmark_tls:
	OK = 0;
#line 103 "cli.gaa"
{ benchmark_tls(gaaval->debug); exit(0) ;};

		return GAA_OK;
		break;
	case GAAOPTID_benchmark_ciphers:
	OK = 0;
#line 102 "cli.gaa"
{ benchmark_cipher(gaaval->debug); exit(0) ;};

		return GAA_OK;
		break;
	case GAAOPTID_insecure:
	OK = 0;
#line 100 "cli.gaa"
{ gaaval->insecure = 1 ;};

		return GAA_OK;
		break;
	case GAAOPTID_port:
	OK = 0;
		GAA_TESTMOREARGS;
		GAA_FILL(GAATMP_port.arg1, gaa_getstr, GAATMP_port.size1);
		gaa_index++;
#line 97 "cli.gaa"
{ gaaval->port = GAATMP_port.arg1 ;};

		return GAA_OK;
		break;
	case GAAOPTID_pskkey:
	OK = 0;
		GAA_TESTMOREARGS;
		GAA_FILL(GAATMP_pskkey.arg1, gaa_getstr, GAATMP_pskkey.size1);
		gaa_index++;
#line 94 "cli.gaa"
{ gaaval->psk_key = GAATMP_pskkey.arg1 ;};

		return GAA_OK;
		break;
	case GAAOPTID_pskusername:
	OK = 0;
		GAA_TESTMOREARGS;
		GAA_FILL(GAATMP_pskusername.arg1, gaa_getstr, GAATMP_pskusername.size1);
		gaa_index++;
#line 91 "cli.gaa"
{ gaaval->psk_username = GAATMP_pskusername.arg1 ;};

		return GAA_OK;
		break;
	case GAAOPTID_srppasswd:
	OK = 0;
		GAA_TESTMOREARGS;
		GAA_FILL(GAATMP_srppasswd.arg1, gaa_getstr, GAATMP_srppasswd.size1);
		gaa_index++;
#line 88 "cli.gaa"
{ gaaval->srp_passwd = GAATMP_srppasswd.arg1 ;};

		return GAA_OK;
		break;
	case GAAOPTID_srpusername:
	OK = 0;
		GAA_TESTMOREARGS;
		GAA_FILL(GAATMP_srpusername.arg1, gaa_getstr, GAATMP_srpusername.size1);
		gaa_index++;
#line 85 "cli.gaa"
{ gaaval->srp_username = GAATMP_srpusername.arg1 ;};

		return GAA_OK;
		break;
	case GAAOPTID_x509certfile:
	OK = 0;
		GAA_TESTMOREARGS;
		GAA_FILL(GAATMP_x509certfile.arg1, gaa_getstr, GAATMP_x509certfile.size1);
		gaa_index++;
#line 82 "cli.gaa"
{ gaaval->x509_certfile = GAATMP_x509certfile.arg1 ;};

		return GAA_OK;
		break;
	case GAAOPTID_x509keyfile:
	OK = 0;
		GAA_TESTMOREARGS;
		GAA_FILL(GAATMP_x509keyfile.arg1, gaa_getstr, GAATMP_x509keyfile.size1);
		gaa_index++;
#line 79 "cli.gaa"
{ gaaval->x509_keyfile = GAATMP_x509keyfile.arg1 ;};

		return GAA_OK;
		break;
	case GAAOPTID_pgpsubkey:
	OK = 0;
		GAA_TESTMOREARGS;
		GAA_FILL(GAATMP_pgpsubkey.arg1, gaa_getstr, GAATMP_pgpsubkey.size1);
		gaa_index++;
#line 76 "cli.gaa"
{ gaaval->pgp_subkey = GAATMP_pgpsubkey.arg1 ;};

		return GAA_OK;
		break;
	case GAAOPTID_pgpcertfile:
	OK = 0;
		GAA_TESTMOREARGS;
		GAA_FILL(GAATMP_pgpcertfile.arg1, gaa_getstr, GAATMP_pgpcertfile.size1);
		gaa_index++;
#line 73 "cli.gaa"
{ gaaval->pgp_certfile = GAATMP_pgpcertfile.arg1 ;};

		return GAA_OK;
		break;
	case GAAOPTID_pgpkeyring:
	OK = 0;
		GAA_TESTMOREARGS;
		GAA_FILL(GAATMP_pgpkeyring.arg1, gaa_getstr, GAATMP_pgpkeyring.size1);
		gaa_index++;
#line 70 "cli.gaa"
{ gaaval->pgp_keyring = GAATMP_pgpkeyring.arg1 ;};

		return GAA_OK;
		break;
	case GAAOPTID_pgpkeyfile:
	OK = 0;
		GAA_TESTMOREARGS;
		GAA_FILL(GAATMP_pgpkeyfile.arg1, gaa_getstr, GAATMP_pgpkeyfile.size1);
		gaa_index++;
#line 67 "cli.gaa"
{ gaaval->pgp_keyfile = GAATMP_pgpkeyfile.arg1 ;};

		return GAA_OK;
		break;
	case GAAOPTID_x509crlfile:
	OK = 0;
		GAA_TESTMOREARGS;
		GAA_FILL(GAATMP_x509crlfile.arg1, gaa_getstr, GAATMP_x509crlfile.size1);
		gaa_index++;
#line 64 "cli.gaa"
{ gaaval->x509_crlfile = GAATMP_x509crlfile.arg1 ;};

		return GAA_OK;
		break;
	case GAAOPTID_x509cafile:
	OK = 0;
		GAA_TESTMOREARGS;
		GAA_FILL(GAATMP_x509cafile.arg1, gaa_getstr, GAATMP_x509cafile.size1);
		gaa_index++;
#line 61 "cli.gaa"
{ gaaval->x509_cafile = GAATMP_x509cafile.arg1 ;};

		return GAA_OK;
		break;
	case GAAOPTID_priority:
	OK = 0;
		GAA_TESTMOREARGS;
		GAA_FILL(GAATMP_priority.arg1, gaa_getstr, GAATMP_priority.size1);
		gaa_index++;
#line 58 "cli.gaa"
{ gaaval->priorities = GAATMP_priority.arg1 ;};

		return GAA_OK;
		break;
	case GAAOPTID_verbose:
	OK = 0;
#line 55 "cli.gaa"
{ gaaval->verbose = 1 ;};

		return GAA_OK;
		break;
	case GAAOPTID_recordsize:
	OK = 0;
		GAA_TESTMOREARGS;
		GAA_FILL(GAATMP_recordsize.arg1, gaa_getint, GAATMP_recordsize.size1);
		gaa_index++;
#line 52 "cli.gaa"
{ gaaval->record_size = GAATMP_recordsize.arg1 ;};

		return GAA_OK;
		break;
	case GAAOPTID_print_cert:
	OK = 0;
#line 49 "cli.gaa"
{ gaaval->print_cert = 1 ;};

		return GAA_OK;
		break;
	case GAAOPTID_disable_extensions:
	OK = 0;
#line 46 "cli.gaa"
{ gaaval->disable_extensions = 1 ;};

		return GAA_OK;
		break;
	case GAAOPTID_fingerprint:
	OK = 0;
#line 43 "cli.gaa"
{ gaaval->fingerprint = 1 ;};

		return GAA_OK;
		break;
	case GAAOPTID_x509fmtder:
	OK = 0;
#line 40 "cli.gaa"
{ gaaval->fmtder = 1 ;};

		return GAA_OK;
		break;
	case GAAOPTID_crlf:
	OK = 0;
#line 37 "cli.gaa"
{ gaaval->crlf = 1 ;};

		return GAA_OK;
		break;
	case GAAOPTID_mtu:
	OK = 0;
		GAA_TESTMOREARGS;
		GAA_FILL(GAATMP_mtu.arg1, gaa_getint, GAATMP_mtu.size1);
		gaa_index++;
#line 34 "cli.gaa"
{ gaaval->mtu = GAATMP_mtu.arg1 ;};

		return GAA_OK;
		break;
	case GAAOPTID_udp:
	OK = 0;
#line 31 "cli.gaa"
{ gaaval->udp = 1 ;};

		return GAA_OK;
		break;
	case GAAOPTID_starttls:
	OK = 0;
#line 28 "cli.gaa"
{ gaaval->starttls = 1 ;};

		return GAA_OK;
		break;
	case GAAOPTID_noticket:
	OK = 0;
#line 25 "cli.gaa"
{ gaaval->noticket = 1 ;};

		return GAA_OK;
		break;
	case GAAOPTID_rehandshake:
	OK = 0;
#line 22 "cli.gaa"
{ gaaval->rehandshake = 1 ;};

		return GAA_OK;
		break;
	case GAAOPTID_resume:
	OK = 0;
#line 19 "cli.gaa"
{ gaaval->resume = 1 ;};

		return GAA_OK;
		break;
	case GAAOPTID_debug:
	OK = 0;
		GAA_TESTMOREARGS;
		GAA_FILL(GAATMP_debug.arg1, gaa_getint, GAATMP_debug.size1);
		gaa_index++;
#line 16 "cli.gaa"
{ gaaval->debug = GAATMP_debug.arg1 ;};

		return GAA_OK;
		break;
	case GAA_REST:
		GAA_TESTMOREARGS;
		GAA_FILL(GAAREST_tmp.arg1, gaa_getstr, GAAREST_tmp.size1);
		gaa_index++;
#line 111 "cli.gaa"
{ gaaval->rest_args = GAAREST_tmp.arg1; ;};

		return GAA_OK;
		break;

#line 413 "gaa.skel"
    default: break;
    }
    return GAA_ERROR_UNKNOWN;
}

int gaa(int argc, char **argv, gaainfo *gaaval)
{
    int tmp1, tmp2;
    int i, j;
    char *opt_list;

    GAAargv = argv;
    GAAargc = argc;

    opt_list = (char*) gaa_malloc(GAA_NB_OPTION + 1);

    for(i = 0; i < GAA_NB_OPTION + 1; i++)
        opt_list[i] = 0;
    /* initialization */
    if(inited == 0)
    {

#line 113 "cli.gaa"
{ gaaval->resume=0; gaaval->noticket=0; gaaval->port="443"; gaaval->rest_args=NULL; 
	gaaval->record_size=0; 
	gaaval->fingerprint=0; gaaval->pgp_keyring=NULL; gaaval->x509_crlfile = NULL;
	gaaval->x509_cafile = NULL; gaaval->pgp_keyfile=NULL; gaaval->pgp_certfile=NULL; gaaval->disable_extensions = 0;
	gaaval->x509_keyfile=NULL; gaaval->x509_certfile=NULL; gaaval->crlf = 0; 
	gaaval->srp_username=NULL; gaaval->srp_passwd=NULL; gaaval->fmtder = 0; gaaval->starttls =0; 
	gaaval->debug = 0; gaaval->print_cert = 0; gaaval->verbose = 0; gaaval->psk_key = NULL; 
	gaaval->psk_username = NULL; gaaval->priorities = NULL; gaaval->mtu = 1300;
	gaaval->pgp_subkey = NULL; gaaval->rehandshake = 0; gaaval->udp = 0; ;};

    }
    inited = 1;
#line 438 "gaa.skel"
    gaa_arg_used = NULL;

    if (argc > 0) {
      gaa_arg_used = gaa_malloc(argc * sizeof(char));
    }

    for(i = 1; i < argc; i++)
        gaa_arg_used[i] = 0;
    for(i = 1; i < argc; i++)
    {
        if(gaa_arg_used[i] == 0)
        {
            j = 0;
            tmp1 = gaa_is_an_argument(GAAargv[i]);
            switch(tmp1)
            {
            case GAA_WORD_OPTION:
                j++;
            case GAA_LETTER_OPTION:
                j++;
                tmp2 = gaa_get_option_num(argv[i]+j, tmp1);
                if(tmp2 == GAA_ERROR_NOMATCH)
                {
                    printf("Invalid option '%s'\n", argv[i]+j);
                    return 0;
                }
                switch(gaa_try(tmp2, i+1, gaaval, opt_list))
                {
                case GAA_ERROR_NOTENOUGH_ARGS:
                    printf("'%s': not enough arguments\n",gaa_current_option);
                    return 0;
                case GAA_ERROR_INVALID_ARG:
                    printf("Invalid arguments\n");
                    return 0;
                case GAA_OK:
                    break;
                default:
                    printf("Unknown error\n");
                }
                gaa_arg_used[i] = 1;
                break;
            case GAA_MULTIPLE_OPTION:
                for(j = 1; j < strlen(argv[i]); j++)
                {
                    tmp2 = gaa_get_option_num(argv[i]+j, tmp1);
                    if(tmp2 == GAA_ERROR_NOMATCH)
                    {
                        printf("Invalid option '%c'\n", *(argv[i]+j));
                        return 0;
                    }
                    switch(gaa_try(tmp2, i+1, gaaval, opt_list))
                    {
                    case GAA_ERROR_NOTENOUGH_ARGS:
                        printf("'%s': not enough arguments\n",gaa_current_option);
                        return 0;
                    case GAA_ERROR_INVALID_ARG:
                        printf("Invalid arguments\n");
                        return 0;
                    case GAA_OK:
                        break;
                    default:
                        printf("Unknown error\n");
                    }
                }
                gaa_arg_used[i] = 1;
                break;
            default: break;
            }
        }
    }
if(gaa_processing_file == 0)
{

#line 507 "gaa.skel"
#ifdef GAA_REST_EXISTS
    switch(gaa_try(GAA_REST, 1, gaaval, opt_list))
    {
    case GAA_ERROR_NOTENOUGH_ARGS:
        printf("Rest: not enough arguments\n");
        return 0;
    case GAA_ERROR_INVALID_ARG:
        printf("Invalid arguments\n");
        return 0;
    case GAA_OK:
        break;
    default:
        printf("Unknown error\n");
    }
#endif
}
    for(i = 1; i < argc; i++)
    {
        if(gaa_arg_used[i] == 0)
        {
            printf("Too many arguments\n");
            return 0;
        }
    }
    free(gaa_arg_used);
    free(opt_list);
    return -1;
}

struct gaastrnode
{
    char *str;
    struct gaastrnode *next;
};

typedef struct gaastrnode gaa_str_node;

static int gaa_internal_get_next_str(FILE *file, gaa_str_node *tmp_str, int argc)
{
    int pos_ini;
    int a;
    int i = 0, len = 0, newline = 0;

    if(argc == 1) {
        newline = 1;
        len = 2;
    }
    
    a = fgetc( file);
    if (a == EOF) return 0;

    while(a == ' ' || a == 9 || a == '\n')
    {
        if(a == '\n')
        {
            newline=1;
            len = 2;
        }
        a = fgetc( file);
        if (a == EOF) return 0;
    }

    pos_ini = ftell(file) - 1;

    while(a != ' ' && a != 9 && a != '\n')
    {

        len++;
        a = fgetc( file);
        if(a==EOF) return 0;
    }

    len += 1;
    tmp_str->str = gaa_malloc((len) * sizeof(char));

    if(newline == 1)
    {
        tmp_str->str[0] = '-';
        tmp_str->str[1] = '-';
        i = 2;
    }
    else
    {
        i = 0;
    }

    fseek(file,pos_ini, SEEK_SET);
    do
    {
        a = fgetc( file);

        if (a == EOF) {
            i+=2;
            break;
        }
        tmp_str->str[i] = a;
        i++;
    }
    while(a != ' ' && a != 9 && a != '\n' && i < len);

    tmp_str->str[i - 1] = 0;

    fseek(file,- 1, SEEK_CUR);
/*    printf("%d\n", ftell(file)); */
    
    return -1;
}

int gaa_file(const char *name, gaainfo *gaaval)
{
    gaa_str_node *first_str, **tmp_str, *tmp_str2;
    int rval, i;
    char **argv;
    int argc = 0;
    FILE *file;

    gaa_processing_file = 1;
    
    if((file = fopen(name, "r")) == NULL)
    {
        printf("Couldn't open '%s' configuration file for reading\n", name);
        return 1;
    }
    
    tmp_str = &first_str;
    do
    {
        argc++;
        *tmp_str = gaa_malloc(sizeof(gaa_str_node));

        (*tmp_str)->str = NULL;
        (*tmp_str)->next = NULL;

        rval = gaa_internal_get_next_str(file, *tmp_str, argc);
        tmp_str = &((*tmp_str)->next);
    }
    while(rval == -1);
    
    if(rval == 1)
        return 0;
    
    argv = gaa_malloc((1 + argc) * sizeof(char*));

    tmp_str2 = first_str;
    argv[0] = "cfg";
    for(i = 1; i < argc; i++)
    {
        argv[i] = tmp_str2->str;
        tmp_str2 = tmp_str2->next;
    }

    rval = gaa(argc, argv, gaaval);
    gaa_processing_file = 0;
    return rval;
}
