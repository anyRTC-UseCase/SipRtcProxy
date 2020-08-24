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
#ifndef __XUTIL_H__
#define __XUTIL_H__

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <stdint.h>
#include <string>
#include <vector>

uint32_t XGetTimestamp(void);
int64_t XGetUtcTimestamp(void);
uint16_t XGetLastSequence(uint16_t usSequence1,uint16_t usSequence2);
uint32_t XGenerateSSRC(void);
void XSleep(uint32_t ulMS);

int XSplitChar(const std::string& source, char delimiter,std::vector<std::string>* fields);

#endif 