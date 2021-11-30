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
#ifndef __RTC_CALL_H__
#define __RTC_CALL_H__
#include <map>
#include <string>
#include "ARtS.h"
#include "IRtcAudCodec.h"
#include "XAutoLock.h"
#include "Timer10Ms.h"


class RtcCall : public ARtSEvent, public Timer10MsEvent, public RtcAudEncoderEvent, public AudNeqDecoderEvent
{//Rtc√ΩÛwÕ®‘íµƒª˘Óê
public:
	RtcCall();
	virtual ~RtcCall(void);

	void SetServer(const std::string&strAddr, int nPort);
	void JoinRtc(const std::string&strAppId, const std::string&strChanId, const std::string&strUId);
	void LeaveRtc();

	virtual void OnRtcCallAudioMixData(const char*pData, int nLen, int nSampleHz, int nChannels, uint32_t timestamp) = 0;

	//* For ARtSEvent
	virtual void on_user_joined(const char *channel, const char * uid, int elapsed);
	virtual void on_user_offline(const char *channel, const char * uid, int reason);
	virtual void on_audio_data(const char *channel, const char * uid, uint32_t sent_ts, uint16_t seqn, uint8_t codec, const void *data, size_t len);

	//* For Timer10MsEvent
	virtual void OnTimer10Ms(int64_t curTime64);

	//* For RtcAudEncoderEvent
	virtual void OnAudioEncoderData(const char*pData, int nLen);

	//* For AudNeqDecoderEvent
	virtual void OnAudNeqDecoderData(const char*strIdd, void* audioSamples, uint32_t samplesPerSec, size_t nChannels);

private:
	ARtS	*rtc_chan_;
	bool	b_join_chan_;
	std::string str_svr_addr_;
	int		n_svr_port_;

	char*	aud_rtc_2_sip_buffer_;
	int		aud_sip_sample_hz_;
	int		aud_sip_channels_;
	int		aud_sip_cur_len_;
	int		aud_sip_req_len_;
protected:
	RtcAudEncoder	*aud_enc_;
	int		aud_rtc_sample_hz_;
	int		aud_rtc_channels_;
private:

	typedef std::map<std::string, NeteqDecoder*> MapNeteqDecoder;
	XCritSec cs_aud_dec_;
	MapNeteqDecoder map_aud_dec_;

};

#endif	// __RTC_CALL_H__