/// @file
/// @brief DPG Stream Connection Handler implementation
///
///  Copyright (c) 2009 by Spirent Communications Inc.
///  All Rights Reserved.
///
///  This software is confidential and proprietary to Spirent Communications Inc.
///  No part of this software may be reproduced, transmitted, disclosed or used
///  in violation of the Software License Agreement without the expressed
///  written consent of Spirent Communications Inc.
///

#include <utils/MessageUtils.h>
#include "DpgClientRawStats.h"
#include "DpgConnectionHandlerStates.h"
#include "DpgStreamConnectionHandler.h"

USING_DPG_NS;

DpgStreamConnectionHandler::DpgStreamConnectionHandler(uint32_t serial)
    : L4L7_APP_NS::StreamSocketHandler(serial),
      DpgConnectionHandler()
{
    RegisterUpdateTxCountDelegate(boost::bind(&DpgConnectionHandler::UpdateGoodputTxSent, this, _1));
    L4L7_APP_NS::StreamSocketHandler::SetCloseNotificationDelegate(fastdelegate::MakeDelegate(this, &DpgStreamConnectionHandler::CloseNotification));
}

///////////////////////////////////////////////////////////////////////////////

DpgStreamConnectionHandler::~DpgStreamConnectionHandler()
{
    UnregisterUpdateTxCountDelegate();
}

///////////////////////////////////////////////////////////////////////////////

int DpgStreamConnectionHandler::HandleOpenHook(void)
{
    return 0;
}

///////////////////////////////////////////////////////////////////////////////

int DpgStreamConnectionHandler::HandleInputHook(void)
{
    // Receive pending data into message blocks and append to current input buffer
    messagePtr_t mb(Recv());
    if (!mb)
        return -1;

    const ACE_Time_Value now(ACE_OS::gettimeofday());

    mInBuffer = L4L7_UTILS_NS::MessageAppend(mInBuffer, mb);

    // Update receive stats
    if (mStats)
    {
        const size_t rxBytes = mb->total_length();
        ACE_GUARD_RETURN(stats_t::lock_t, guard, mStats->Lock(), -1);
        mStats->goodputRxBytes += rxBytes;
    }

    // Process the input buffer
    int err = 0;
    while (!err)
    {
        const char * const pos = mInBuffer->rd_ptr();

        if (!mState->ProcessInput(*this, now))
            err = -1;

        if (!mInBuffer || pos == mInBuffer->rd_ptr())
            break;
    }

    return err;
}

///////////////////////////////////////////////////////////////////////////////

void DpgStreamConnectionHandler::CloseNotification(L4L7_APP_NS::StreamSocketHandler& socketHandler)
{
  HandleClose(L4L7_ENGINE_NS::AbstSocketManager::RX_FIN /*FIXME CLOSETYPE */);
}

///////////////////////////////////////////////////////////////////////////////
