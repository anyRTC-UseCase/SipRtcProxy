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
#include "RtcCallToSip.h"
#include <assert.h>

RtcCallToSip::RtcCallToSip(RtcCallToSipEvent&callback)
	: sip_call_(NULL)
	, callback_(callback)
	, rtm_channel_(NULL)
	, task_started_(false)
	, b_conference_(false)
	, n_chan_member_size_(0)
	, rtm_chan_check_memsize_time_(0)
{
	
}
RtcCallToSip::~RtcCallToSip(void)
{
	assert(sip_call_ == NULL);
	assert(rtm_channel_ == NULL);
}

void RtcCallToSip::SetIChannel(ARM::IChannel*rtmChannel)
{
	assert(rtm_channel_ == NULL);
	b_conference_ = true;	// P2P不会加入频道，只有会议才会加入频道
	rtm_channel_ = rtmChannel;
	rtm_chan_check_memsize_time_ = XGetUtcTimestamp() + 60000;	// 1 minute later
}
void RtcCallToSip::InitSipAccount(const std::string&strSvrIp, int nPort, const std::string&strAccount, const std::string&strPwd)
{
	str_sip_svr_ = strSvrIp;
	str_sip_account_ = strAccount;
	if (sip_call_ == NULL) {
		sip_call_ = SipCall::Create(*this, strSvrIp, nPort, strAccount, strPwd);
	}
}
void RtcCallToSip::StartTask(const std::string&strAppId, const std::string&strChanId, const std::string&strUserId, const std::string&strSipNumber, const std::string&strSipData)
{
	assert(sip_call_ != NULL);
	task_started_ = true;
	str_chan_id_ = strChanId;
	str_user_id_ = strUserId;
	str_sip_number_ = strSipNumber;
	str_sip_data_ = strSipData;

	RtcCall::JoinRtc(strAppId, strChanId, strUserId);

	if (rtm_channel_ != NULL) {
		rtm_channel_->join();
	}

	aud_enc_->Init(aud_rtc_sample_hz_, aud_rtc_channels_, 24000, false);
}
void RtcCallToSip::StopTask()
{
	task_started_ = false;
	aud_enc_->DeInit();
	RtcCall::LeaveRtc();

	if (sip_call_ != NULL) {
		sip_call_->EndCall();
		SipCall::Destory(sip_call_);
		sip_call_ = NULL;
	}

	if (rtm_channel_ != NULL) {
		rtm_channel_->leave();
		rtm_channel_->release();
		rtm_channel_ = NULL;
	}
}

void RtcCallToSip::DoProcess()
{
	if (rtm_chan_check_memsize_time_ != 0 && rtm_chan_check_memsize_time_ <= XGetUtcTimestamp()) {
		if (n_chan_member_size_ <= 1) {
			rtm_chan_check_memsize_time_ = 0;
			callback_.OnRtcCallToSipClosed(str_caller_id_, -102);
		}
	}
}

//* For RtcCall
void RtcCallToSip::OnRtcCallAudioMixData(const char*pData, int nLen, int nSampleHz, int nChannels, uint32_t timestamp)
{
	if (sip_call_ != NULL) {
		sip_call_->SetAudioData(pData, nLen, nSampleHz, nChannels, timestamp);
	}
}

//* For SipCallEvent
void RtcCallToSip::OnRegState(bool Ok)
{
	if (Ok) {
		char strUrl[256];
		sprintf(strUrl, "sip:%s@%s", str_sip_number_.c_str(), str_sip_svr_.c_str());
		sip_call_->MakeCall(strUrl, str_sip_number_, str_sip_data_, false);
	}
	else {
		if (task_started_) {
			callback_.OnRtcCallToSipClosed(str_caller_id_, -1);
		}
	}
}
void RtcCallToSip::OnCallOk()
{
	
}
void RtcCallToSip::OnCallHangUp()
{
	if (task_started_) {
		callback_.OnRtcCallToSipClosed(str_caller_id_, 0);
	}
}
void RtcCallToSip::OnCallAudioData(const char*pData, int nLen, int nSampleHz, int nChannels)
{
	int aFrame10Ms = nSampleHz * nChannels * sizeof(int16_t) / 100;
	int bufferSize = nLen;
	int bufferUsed = 0;
	while (bufferSize > 0)
	{
		//memset((char*)pData + bufferUsed, 0, aFrame10Ms)
		aud_enc_->SetAudioData(pData + bufferUsed, nSampleHz, nChannels);

		bufferSize -= aFrame10Ms;
		bufferUsed += aFrame10Ms;
	}
}

//* For ARM::IChannelEventHandler
void RtcCallToSip::onJoinSuccess()
{

}
void RtcCallToSip::onJoinFailure(ARM::JOIN_CHANNEL_ERR errorCode)
{
	rtm_chan_check_memsize_time_ = 0;
	callback_.OnRtcCallToSipClosed(str_caller_id_, -100);
}
void RtcCallToSip::onLeave(ARM::LEAVE_CHANNEL_ERR errorCode)
{
	rtm_chan_check_memsize_time_ = 0;
	callback_.OnRtcCallToSipClosed(str_caller_id_, -101);
}
void RtcCallToSip::onMemberCountUpdated(int memberCount)
{// 由于是服务端，没有主动挂断按钮可操作，所以需要对频道内人员数进行判断。当频道内只剩自己一个人时则退出频道
	n_chan_member_size_ = memberCount;
}

