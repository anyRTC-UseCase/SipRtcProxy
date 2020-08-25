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
#ifndef __RTC_CALL_TO_SIP_H__
#define __RTC_CALL_TO_SIP_H__
#include "SipCall.h"
#include "RtcCall.h"
#include "IArRtmService.h"

class RtcCallToSipEvent
{
public:
	RtcCallToSipEvent(void) {};
	virtual ~RtcCallToSipEvent(void) {};

	// 呼叫结束
	virtual void OnRtcCallToSipClosed(const std::string&strCallerId, int nCode) = 0;
};

/** RtcCallToSip类
 * 实现Rtc主动呼叫Sip的数据互通
 * 
 */
class RtcCallToSip : public RtcCall, public SipCallEvent, public ARM::IChannelEventHandler
{
public:
	RtcCallToSip(RtcCallToSipEvent&callback);
	virtual ~RtcCallToSip(void);

	//* 发起端的UserId
	void SetCallerId(const std::string&strCallerId) { str_caller_id_ = strCallerId; };
	//* 是否是多人会议模式
	bool IsConference() { return b_conference_; };
	//* Sip的账号
	const std::string&SipAccount() { return str_sip_account_; };
	//* Sip的账号
	const std::string&ChanId() { return str_chan_id_; };
	//* 设置多人呼叫的Channel
	void SetIChannel(ARM::IChannel*rtmChannel);

	void InitSipAccount(const std::string&strSvrIp, int nPort, const std::string&strAccount, const std::string&strPwd);
	void StartTask(const std::string&strAppId, const std::string&strChanId, const std::string&strSipNumber, const std::string&strSipData);
	void StopTask();

	void DoProcess();

	//* For RtcCall
	virtual void OnRtcCallAudioMixData(const char*pData, int nLen, int nSampleHz, int nChannels, uint32_t timestamp);

	//* For SipCallEvent
	virtual void OnRegState(bool Ok);
	virtual void OnCallOk();
	virtual void OnCallHangUp();
	virtual void OnCallAudioData(const char*pData, int nLen, int nSampleHz, int nChannels);

	//* For ARM::IChannelEventHandler
	virtual void onJoinSuccess();
	virtual void onJoinFailure(ARM::JOIN_CHANNEL_ERR errorCode);
	virtual void onLeave(ARM::LEAVE_CHANNEL_ERR errorCode);
	virtual void onMemberCountUpdated(int memberCount);

private:
	SipCall	*sip_call_;
	RtcCallToSipEvent &callback_;
	ARM::IChannel	*rtm_channel_;
	bool		task_started_;
	bool		b_conference_;
	int			n_chan_member_size_;
	int64_t		rtm_chan_check_memsize_time_;		// 用于Rtm的频道检测频道里面的人员数量，如果只有自己则需要退出频道
	std::string str_caller_id_;
	std::string str_chan_id_;
	std::string str_sip_svr_;
	std::string str_sip_account_;
	std::string str_sip_number_;
	std::string str_sip_data_;
};

#endif	// __RTC_CALL_TO_SIP_H__

