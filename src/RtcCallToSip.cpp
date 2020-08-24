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
	, task_started_(false)
{
	
}
RtcCallToSip::~RtcCallToSip(void)
{
	assert(sip_call_ == NULL);
}

void RtcCallToSip::InitSipAccount(const std::string&strSvrIp, int nPort, const std::string&strAccount, const std::string&strPwd)
{
	str_sip_svr_ = strSvrIp;
	str_sip_account_ = strAccount;
	if (sip_call_ == NULL) {
		sip_call_ = SipCall::Create(*this, strSvrIp, nPort, strAccount, strPwd);
	}
}
void RtcCallToSip::StartTask(const std::string&strAppId, const std::string&strChanId, const std::string&strSipNumber, const std::string&strSipData)
{
	assert(sip_call_ != NULL);
	task_started_ = true;
	str_sip_number_ = strSipNumber;
	str_sip_data_ = strSipData;

	RtcCall::JoinRtc(strAppId, strChanId, strSipNumber);
}
void RtcCallToSip::StopTask()
{
	task_started_ = false;
	RtcCall::LeaveRtc();

	if (sip_call_ != NULL) {
		sip_call_->EndCall();
		SipCall::Destory(sip_call_);
		sip_call_ = NULL;
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
	aud_enc_->Init(aud_rtc_sample_hz_, aud_rtc_channels_, 32000, false);
}
void RtcCallToSip::OnCallHangUp()
{
	aud_enc_->DeInit();

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
		aud_enc_->SetAudioData(pData + bufferUsed, nSampleHz, nChannels);

		bufferSize -= aFrame10Ms;
		bufferUsed += aFrame10Ms;
	}
}

