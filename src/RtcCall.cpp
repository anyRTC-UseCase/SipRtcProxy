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
#include "RtcCall.h"
#include <assert.h>
static const size_t kMaxDataSizeSamples = 3840;

RtcCall::RtcCall()
	: rtc_chan_(NULL)
	, b_join_chan_(false)
	, n_svr_port_(0)
	, aud_rtc_2_sip_buffer_(NULL)
	, aud_sip_sample_hz_(16000)
	, aud_sip_channels_(1)
	, aud_sip_cur_len_(0)
	, aud_sip_req_len_(0)
	, aud_enc_(NULL)
	, aud_rtc_sample_hz_(48000)
	, aud_rtc_channels_(2)
{
	aud_rtc_2_sip_buffer_ = new char[kMaxDataSizeSamples];
	aud_enc_ = createRtcAudEncoder(*this);
	rtc_chan_ = createARtSEngine();

	aud_sip_req_len_ = (aud_sip_sample_hz_ * aud_sip_channels_ * sizeof(int16_t)) / 50;
}
RtcCall::~RtcCall(void)
{
	assert(!b_join_chan_);

	//#3 The C++ language guarantees that delete p will do nothing if p is null
	{
		delete[] aud_rtc_2_sip_buffer_;
		aud_rtc_2_sip_buffer_ = NULL;
	}
	if(aud_enc_ != NULL)
	{
		aud_enc_->DeInit();
		delete aud_enc_;
		aud_enc_ = NULL;
	}
	{
		delete rtc_chan_;
		rtc_chan_ = NULL;
	}
}

void RtcCall::SetServer(const std::string&strAddr, int nPort)
{
	str_svr_addr_ = strAddr;
	n_svr_port_ = nPort;
}
void RtcCall::JoinRtc(const std::string&strAppId, const std::string&strChanId, const std::string&strUId)
{
	if (!b_join_chan_) {
		b_join_chan_ = true;
		rtc_chan_->init(strAppId.c_str(), strUId.c_str(), this, NULL);
		char strParams[512];
		sprintf(strParams, "{\"Cmd\":\"SetStreamTypes\", \"StreamTypes\": %d}", 0/*0:audio 1:video 2:audio&video*/);
		rtc_chan_->set_parameters(strParams);
		rtc_chan_->set_parameters("{\"Cmd\":\"SetHost\", \"Host\": true}");
		if (str_svr_addr_.length() > 0 && n_svr_port_ > 0) {
			sprintf(strParams, "{\"Cmd\":\"ConfPriCloudAddr\", \"ServerAdd\": \"%s\", \"Port\": %d}", str_svr_addr_.c_str(), n_svr_port_);
			rtc_chan_->set_parameters(strParams);
		}
		rtc_chan_->join_channel(strChanId.c_str(), NULL, 0);
		rtc_chan_->mute_local_video(true);	// ²»°lËÍÒ•îl”µ“þ

		Timer10Ms::Inst().RegisterTimer(this, this);
	}
}

	
void RtcCall::LeaveRtc()
{
	if (b_join_chan_) {
		b_join_chan_ = false;
		
		Timer10Ms::Inst().UnRegisterTimer(this);
		rtc_chan_->leave_channel();
	}
}

//* For 
void RtcCall::on_user_joined(const char *channel, const char * uid, int elapsed)
{
	XAutoLock l(cs_aud_dec_);
	if (map_aud_dec_.find(uid) == map_aud_dec_.end()) {
		NeteqDecoder* audDec = createNeteqDecoder(uid, *this);
		map_aud_dec_[uid] = audDec;
	}
}
void RtcCall::on_user_offline(const char *channel, const char * uid, int reason)
{
	XAutoLock l(cs_aud_dec_);
	if (map_aud_dec_.find(uid) != map_aud_dec_.end()) {
		NeteqDecoder* audDec = map_aud_dec_[uid];
		map_aud_dec_.erase(uid);

		delete audDec;
		audDec = NULL;
	}

}
void RtcCall::on_audio_data(const char *channel, const char * uid, uint32_t sent_ts, uint16_t seqn, uint8_t codec, const void *data, size_t len)
{
	XAutoLock l(cs_aud_dec_);
	if (map_aud_dec_.find(uid) != map_aud_dec_.end()) {
		NeteqDecoder* audDec = map_aud_dec_[uid];

		audDec->SetAudEncData((char*)data, len, seqn, sent_ts);
	}
}

//* For Timer10MsEvent
void RtcCall::OnTimer10Ms(int64_t curTime64)
{
	bool bMix = false;
	if (aud_sip_cur_len_ == 0) {
		memset(aud_rtc_2_sip_buffer_, 0, kMaxDataSizeSamples);
	}
	

	{
		XAutoLock l(cs_aud_dec_);
		MapNeteqDecoder::iterator itnr = map_aud_dec_.begin();
		while (itnr != map_aud_dec_.end()) {
			NeteqDecoder* audDec = itnr->second;
			if (audDec->MixAudPcmData(bMix, aud_rtc_2_sip_buffer_ + aud_sip_cur_len_, aud_sip_sample_hz_, aud_sip_channels_) > 0) {
				bMix = true;
			}
			itnr++;
		}
	}

	aud_sip_cur_len_ += aud_sip_sample_hz_ * sizeof(int16_t) * aud_sip_channels_ / 100;
	if (aud_sip_cur_len_ >= aud_sip_req_len_) {
		OnRtcCallAudioMixData(aud_rtc_2_sip_buffer_, aud_sip_cur_len_, aud_sip_sample_hz_, aud_sip_channels_, XGetTimestamp());
		aud_sip_cur_len_ = 0;
	}
}

//* For RtcAudEncoderEvent
void RtcCall::OnAudioEncoderData(const char*pData, int nLen)
{
	if (rtc_chan_ != NULL) {
		rtc_chan_->send_audio_data(0, pData, nLen);
	}
}

//* For AudNeqDecoderEvent
void RtcCall::OnAudNeqDecoderData(const char*strIdd, void* audioSamples, uint32_t samplesPerSec, size_t nChannels)
{//* Do nothing
}
