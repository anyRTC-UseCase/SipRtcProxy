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
#ifndef __XAUTOLOCK_H__
#define __XAUTOLOCK_H__

#include "XCritSec.h"

class XAutoLock  
{
public:
	XAutoLock(XCritSec&rXCritSec);
	~XAutoLock(void);
protected:
    XCritSec& m_rXCritSec;
};

#endif 