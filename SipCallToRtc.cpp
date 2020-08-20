#include "SipCallToRtc.h"

SipCallToRtc::SipCallToRtc(int nCallId, SipProxy*proxy)
	: n_call_id_(nCallId)
	, sip_proxy_(proxy)
{

}
SipCallToRtc::~SipCallToRtc(void)
{

}

void SipCallToRtc::StartTask(const std::string&strAppId, const std::string&strChanId, const std::string&strSipNumber)
{
	RtcCall::JoinRtc(strAppId, strChanId, strSipNumber);

	aud_enc_->Init(aud_rtc_sample_hz_, aud_rtc_channels_, 32000, false);
}
void SipCallToRtc::StopTask()
{
	aud_enc_->DeInit();

	RtcCall::LeaveRtc();
}

void SipCallToRtc::SetSipAudToRtc(const char*pData, int nLen, int nSampleHz, int nChannels)
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

	//aud_enc_->SetAudioData(pData, nSampleHz, nChannels);
}

//* For RtcCall
void SipCallToRtc::OnRtcCallAudioMixData(const char*pData, int nLen, int nSampleHz, int nChannels, uint32_t timestamp)
{
	sip_proxy_->SetRtcAudioToSip(n_call_id_, pData, nLen, nSampleHz, nChannels);
}
