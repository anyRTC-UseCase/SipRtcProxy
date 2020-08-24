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
#include "Timer10Ms.h"

Timer10Ms&Timer10Ms::Inst()
{
	static Timer10Ms gInst;
	return gInst;
}
Timer10Ms::Timer10Ms(void)
	: b_running_(false)
{
	last_tick_time64_ = XGetUtcTimestamp() + 10;
	b_running_ = true;
	XThreadBase::StartThread();
}
Timer10Ms::~Timer10Ms(void)
{
	if (b_running_) {
		b_running_ = false;
		XThreadBase::WaitForStop();
	}
}

void Timer10Ms::RegisterTimer(void* ptr, Timer10MsEvent* pEvent)
{
	XAutoLock l(cs_map_timer_);
	if (map_timer_.find(ptr) == map_timer_.end()) {
		map_timer_[ptr] = pEvent;
	}
}
void Timer10Ms::UnRegisterTimer(void* ptr)
{
	XAutoLock l(cs_map_timer_);
	if (map_timer_.find(ptr) != map_timer_.end()) {
		map_timer_.erase(ptr);
	}
}

//* For XThreadBase
void Timer10Ms::ThreadProcMain(void)
{
	while (b_running_)
	{
		int64_t curTime64 = XGetUtcTimestamp();
		if (last_tick_time64_ <= curTime64) {
			last_tick_time64_ += 10;

			XAutoLock l(cs_map_timer_);
			MapTimer10MsEvent::iterator ittr = map_timer_.begin();
			while (ittr != map_timer_.end()) {
				ittr->second->OnTimer10Ms(curTime64);
				ittr++;
			}
		}
		XSleep(1);
	}
}
