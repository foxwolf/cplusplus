/* rvadtstamp.c - Linux (x86) adapter timestamp functions */
/************************************************************************
        Copyright (c) 2001 RADVISION Inc. and RADVISION Ltd.
************************************************************************
NOTICE:
This document contains information that is confidential and proprietary
to RADVISION Inc. and RADVISION Ltd.. No part of this document may be
reproduced in any form whatsoever without written prior approval by
RADVISION Inc. or RADVISION Ltd..

RADVISION Inc. and RADVISION Ltd. reserve the right to revise this
publication and make changes without obligation to notify any person of
such revisions or changes.
***********************************************************************/

/* SPIRENT_BEGIN */
#if defined(UPDATED_BY_SPIRENT) && defined(__MIPS__)
#include <posix_time.h>
#include <time.h>

#ifdef UNIT_TEST
#define VOIPCLOCK CLOCK_MONOTONIC
#else
#define VOIPCLOCK CLOCK_MONOTONIC_HR
#endif

#endif
/* SPIRENT_END */

#include "rvadtstamp.h"
#include "rvtime.h"
#include "rvstdio.h"
#include <string.h>

static RvInt64 RvTickHz; /* Clock frequency (cycles/second) - set by Init */
/* SPIRENT_BEGIN */
#if defined(UPDATED_BY_SPIRENT)
static RvInt64 milliseconds_divider=1;
#endif
/* SPIRENT_END */

/* #include <asm/timex.h> --- not installed in Linux 7.3 */
static inline RvUint64 rdtsc()
{
    /* SPIRENT_BEGIN */
#if defined(UPDATED_BY_SPIRENT) && defined(__MIPS__)

    struct timespec tp;
    clock_gettime (VOIPCLOCK, &tp);
    return (__uint64_t)(tp.tv_sec * (__uint64_t)1000000000) + (__uint64_t)tp.tv_nsec;

#else
    /* SPIRENT_END */

#if defined(__ia64)
    /* for Intel Itanium 64 bit processor */
    unsigned long x;
    __asm__ __volatile__("mov %0=ar.itc" : "=r"(x) :: "memory");
    while (__builtin_expect ((int) x == -1, 0))
        __asm__ __volatile__("mov %0=ar.itc" : "=r"(x) :: "memory");
    return x;
#else
    /* for all others */
    RvUint64 x;
    RvUint32 low,high;
    __asm__ __volatile__("rdtsc" : "=a" (low), "=d" (high));
    /*__asm__ volatile ("rdtsc" : "=A" (x1));*/
    x = high;
    x = (x<<32)+low;
    return x;
#endif

    /* SPIRENT_BEGIN */
#endif
    /* SPIRENT_END */
}


RvStatus RvAdTimestampInit(void)
{
    FILE *cpuinfo;
    char linebuf[132], *cptr1, *cptr2;
    RvBool has_tsc, found_mhz;
    long scale, temp;

/* SPIRENT_BEGIN */
#if defined(UPDATED_BY_SPIRENT) && defined(__MIPS__)
    RvTickHz = RvInt64Const(1,0,1000000000); 
    milliseconds_divider = RvInt64Const(1,0,1000000); 
    return RV_OK;
#endif
/* SPIRENT_END */

    /* The only way to find out if the high performance timer (tsc) */
    /* is present and it's frequency is to read it from the file */
    /* /proc/cpuinfo which is generated by the kernel at boot time. */
    has_tsc = RV_FALSE;    /* mark when we have found if tsc is present */
    found_mhz = RV_FALSE;  /* mark when we have found and set the cpu speed */
    RvTickHz = RvInt64Const(1,0,100000000); /* just to prevent accidental divide by zero */
    /* SPIRENT_BEGIN */
#if defined(UPDATED_BY_SPIRENT)
    milliseconds_divider = RvInt64Const(1,0,1000000); 
#endif
/* SPIRENT_END */
    cpuinfo = RvFopen("/proc/cpuinfo", "r");
    if(cpuinfo == NULL)
        return RV_TIMESTAMP_ERROR_NOCPUINFO;

    while(fgets(linebuf, sizeof(linebuf), cpuinfo) != NULL)
    {
        /* check for MHz line which will give clock (and tsc) speed */
        if(strncmp(linebuf, "cpu MHz", 7) == 0) {
            /* line contains cpu speed info, parse it into RvTickHz */
            cptr1 = strchr(linebuf, ':');  /* find start of data field */
            if (cptr1 != NULL) {
                cptr1++;                   /* skip ":" seperator */
                cptr2= strchr(cptr1, '.'); /* find decimal point */
                if(cptr2 != NULL) {        /* it had better be there */
                    *cptr2 = '\0';         /* split number into parts */
                    cptr2++;               /* move to start of decimal */
                    temp = strtol(cptr1, NULL, 0); /* read whole number */
                    if(temp > 0) {         /* sanity check */
                        RvTickHz = RvInt64Const(1,0,1000000) * (RvInt64)temp;
                        /* now add variable lengh fraction amount */
                        scale = 100000L;    /* scale of next digit */
                        while((scale > 0L) && (isdigit(*cptr2) != 0)) {
                            temp = (long)(*cptr2 - '0');
                            RvTickHz += (RvInt64)(temp * scale);
                            scale = scale / 10L;
                            cptr2++;
                        }
                        found_mhz = RV_TRUE;
                    }
                }
            }
        } /* end of cpu MHz check/parse */

#if (RV_ARCH_BITS == RV_ARCH_BITS_64)
		/* Linux 64bit supports the tsc, but does not indicate that on in "/proc/cpuinfo" */
	    has_tsc = RV_TRUE;
#else
        /* check for "flags" line which lists tsc if it is present */
        if(strncmp(linebuf, "flags", 5) == 0) {
            cptr1 = strchr(linebuf, ':'); /* find start of data field */
            if (cptr1 != NULL) {
                cptr2 = strstr(cptr1, " tsc"); /* see if tsc is there */
                if(cptr2 != NULL) has_tsc = RV_TRUE;
            }
        }
#endif
    } /* end of file read loop */
    fclose(cpuinfo);

    /* SPIRENT_BEGIN */
#if defined(UPDATED_BY_SPIRENT)
    milliseconds_divider = RvTickHz / 1000;
#endif
/* SPIRENT_END */

    if (has_tsc == RV_FALSE)
        return RV_TIMESTAMP_ERROR_NOTSC;

    if (found_mhz == RV_FALSE)
        return RV_TIMESTAMP_ERROR_NOMHZ;

    return RV_OK;
}


void RvAdTimestampEnd(void)
{
}

/* SPIRENT_BEGIN */
#if defined(UPDATED_BY_SPIRENT)
RvUint64 _getMilliSeconds(void)
{
  RvUint64 result;
  
  if(milliseconds_divider==1) RvAdTimestampInit();
  
  result = ((RvUint64)rdtsc()) / ((RvUint64)milliseconds_divider);

  return result;
}
#endif
/* SPIRENT_END */

RvInt64 RvAdTimestampGet(void)
{
    RvInt64 result;
    RvInt64 tickcount;

    tickcount = rdtsc();

    /* convert to nanoseconds and maintain resolution, */
    /* works up to about a 9 gigahertz clock */
    result = (tickcount / RvTickHz) * RV_TIME64_NSECPERSEC;
    result += (((tickcount % RvTickHz) * RV_TIME64_NSECPERSEC) / RvTickHz);

    return result;
}


RvInt64 RvAdTimestampResolution(void)
{
    RvInt64 result;

    if(RvInt64IsGreaterThan(RvTickHz, RvInt64Const(1,0,1)))
        result = RvInt64Div(RV_TIME64_NSECPERSEC, RvTickHz);
    else
        result = RV_TIME64_NSECPERSEC;

    return result;
}
