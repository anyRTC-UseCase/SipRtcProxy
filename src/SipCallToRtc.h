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
#ifndef __SIP_CALL_TO_RTC_H__
#define __SIP_CALL_TO_RTC_H__
#include "SipCall.h"
#include "RtcCall.h"

/** SipCallToRtc类
 * 实现Sip主动呼叫Rtc的数据互通
 *
 */
class SipCallToRtc : public RtcCall
{
public:
	SipCallToRtc(int nCallId, SipProxy*proxy);
	virtual ~SipCallToRtc(void);

	void SetCalleeId(const std::string&strCalleeId) { str_callee_id_ = strCalleeId; };
	const std::string&CalleeId() { return str_callee_id_; };

	void StartTask(const std::string&strAppId, const std::string&strChanId, const std::string&strSipNumber);
	void StopTask();

	void SetSipAudToRtc(const char*pData, int nLen, int nSampleHz, int nChannel);

	void SetUserData(int nType, void*pUserData);
	int GetUserType();
	void*GetUserData();

	//* For RtcCall
	virtual void OnRtcCallAudioMixData(const char*pData, int nLen, int nSampleHz, int nChannels, uint32_t timestamp);

private:
	int			n_call_id_;
	SipProxy	*sip_proxy_;

	std::string str_callee_id_;

	int			n_type_;
	void		*user_data_;
};

#endif	// __SIP_CALL_TO_RTC_H__
