/*
 *  Copyright (c) 2020 The anyRTC project authors. All Rights Reserved.
 *
 *  Website: https://www.anyrtc.io for more details.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#include "XAutoLock.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
XAutoLock::XAutoLock(XCritSec&rXCritSec)
:x_crit_sec_(rXCritSec)
{
	x_crit_sec_.Lock();
}

XAutoLock::~XAutoLock()
{
	x_crit_sec_.UnLock();
}
