#ifndef __I_RTC_AUD_CODEC_H__
#define __I_RTC_AUD_CODEC_H__
#include <stdint.h>
#include "ArBase.h"

class AudNeqDecoderEvent
{
public:
	AudNeqDecoderEvent(void) {};
	virtual ~AudNeqDecoderEvent(void) {};

	virtual void OnAudNeqDecoderJitterDelay(const char*strIdd, int nDelay) {};
	virtual void OnAudNeqDecoderData(const char*strIdd, void* audioSamples, uint32_t samplesPerSec, size_t nChannels) = 0;
};

class NeteqDecoder
{
protected:
	NeteqDecoder() {};
public:
	virtual ~NeteqDecoder() {};

	virtual void SetRenderStartTime(uint32_t nTime) = 0;
	virtual uint32_t RenderTime() = 0;
	virtual int DelayTime() = 0;
	virtual bool UseAudTime() = 0;
	virtual bool SetMinimumDelay(int delay_ms) = 0;
	virtual bool SetMaximumDelay(int delay_ms) = 0;

	virtual void SetAudEncData(const char*pData, int nLen, uint16_t nSeqn, uint32_t nTimestamp) = 0;
	virtual int MixAudPcmData(bool mix, int nVolume, void* audioSamples, uint32_t samplesPerSec, size_t nChannels) = 0;
	virtual int MixAudPcmData(bool mix, void* audioSamples, uint32_t samplesPerSec, size_t nChannels) = 0;
};

AR_API NeteqDecoder* AR_CALL createNeteqDecoder(const char*strIdd, AudNeqDecoderEvent&callback, const char*strCodecType = "Opus");

class RtcAudEncoderEvent
{
public:
	RtcAudEncoderEvent(void) {};
	virtual ~RtcAudEncoderEvent(void) {};

	virtual void OnAudioEncoderData(const char*pData, int nLen) = 0;
};


class RtcAudEncoder
{
protected:
	RtcAudEncoder(RtcAudEncoderEvent&callback) :callback_(callback) {};
public:
	virtual ~RtcAudEncoder() {};

	virtual bool Init(int nSampleHz, int nChannel, int nBitrate, bool bMusic) = 0;
	virtual void DeInit() = 0;

	virtual void SetCodecType(const char*strCodecType) = 0;
	virtual void SetEnable(bool enable) = 0;
	virtual bool SetAudioData(const char*pData, int nSampleHz, int nChannels) = 0;

protected:
	RtcAudEncoderEvent& callback_;
};

AR_API RtcAudEncoder* AR_CALL createRtcAudEncoder(RtcAudEncoderEvent&callback);


class RtcAudDecoderEvent
{
public:
	RtcAudDecoderEvent(void) {};
	virtual ~RtcAudDecoderEvent(void) {};

	virtual void OnAudioDecoderData(void*audioSamples, int samplesPerSec, int nChannels, uint32_t nTimestamp) = 0;
};

class RtcAudDecoder
{
protected:
	RtcAudDecoder(RtcAudDecoderEvent&callback) :callback_(callback) {};
public:
	static RtcAudDecoder*Create(RtcAudDecoderEvent&callback);
	virtual ~RtcAudDecoder(void) {};

	//* 如果社会为True，则使用接口GetPlayAudio获取数据，而不通过回调返回数据
	virtual void SetCacheAud(bool bCache) = 0;
	virtual void SetAudioData(bool bContinue, const char*pData, int nLen, uint32_t nTimestamp) = 0;
	virtual int GetPlayAudio(void* audioSamples, uint32_t& samplesPerSec, size_t& nChannels) = 0;

private:
	RtcAudDecoderEvent&callback_;
};

AR_API RtcAudDecoder* AR_CALL createRtcAudDecoder(RtcAudDecoderEvent&callback);


#endif	// __I_RTC_AUD_CODEC_H__
