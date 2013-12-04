/******************************************************************************
Filename    :rvsdpsymb.h
Description :This file exists for backward compatibility.

******************************************************************************
                Copyright (c) 1999 RADVision Inc.
************************************************************************
NOTICE:
This document contains information that is proprietary to RADVision LTD.
No part of this publication may be reproduced in any form whatsoever
without written prior approval by RADVision LTD..

RADVision LTD. reserves the right to revise this publication and make
changes without obligation to notify any person of such revisions or
changes.
******************************************************************************/
#ifndef _rvsdpsymb_h
#define _rvsdpsymb_h

/* For backward compatibility only */
#include "rvsdp.h"

/* SPIRENT_BEGIN */
#if defined(UPDATED_BY_SPIRENT)
const char * rvSdpSymbolGetString(int value,RvSdpStatus* stat);
#endif
/* SPIRENT_END */
#endif

