#ifndef __SIP_CALL_H__
#define __SIP_CALL_H__
#include <string>
#include "RTC_Export.h"

#define MAX_DIGIT 256

class SipCallEvent
{
public:
	SipCallEvent(void) {};
	virtual ~SipCallEvent(void) {};

	virtual void OnRegState(bool Ok) = 0;
	virtual void OnCallOk() = 0;
	virtual void OnCallHangUp() = 0;
	virtual void OnCallAudioData(const char*pData, int nLen, int nSampleHz, int nChannels) = 0;
};

class AR_EXPORT SipCall
{
public:
	static void Init();
	static void Deinit();

	static SipCall*Create(SipCallEvent&event, const std::string&strDomain, int nPort, const std::string&strSipAccount, const std::string&strPwd);
	static void Destory(SipCall*);


	virtual void MakeCall(const std::string&strUri, const std::string&strPhoneNumber, bool bPSTN) = 0;
	virtual void MakeCall(const std::string&strUri, const std::string&strPhoneNumber, const std::string&strArInfo, bool bPSTN) = 0;
	virtual void EndCall() = 0;
	virtual void SetAudioData(const char*pData, int nLen, int nSampleHz, int nChannels, uint32_t timestamp) = 0;

protected:
	SipCall() {};
	virtual ~SipCall() {};
};

class SipProxyEvent
{
public:
	SipProxyEvent(void) {};
	virtual ~SipProxyEvent(void) {};

	virtual void OnSipIncomingCall(int callId, const std::string&strFromSipId, const std::string&strToSipId, int &nDualCall, const std::string&strCusData) = 0;
	virtual void OnSipCallApp(int callId, const std::string&strFromSipId, const std::string&strToSipId, const std::string&strCusData) = 0;
	virtual void OnSipEndCall(int callId) = 0;
	virtual void OnSipNeedPstn(int callId) = 0;
	virtual void OnSipReleasePstn(const std::string&strFxoAccount) = 0;

	virtual void OnRecvSipPeerAudio(int callId, const char*pData, int nLen, int nSampleHz, int nChannel) = 0;
};

class AR_EXPORT SipProxy
{
public:
	static SipProxy*Create(SipProxyEvent&event, const std::string&strDomain, const std::string&strSipAccount, const std::string&strPwd);
	static void Destory(SipProxy*);

	virtual void RtcAcceptCall(int callId) = 0;
	virtual void RtcEndCall(int callId) = 0;
	virtual void EnablePstn(int callId, bool enable) = 0;
	virtual void CallPstn(int callId, const std::string&strFxoAccnout, const std::string&strFxoUri) = 0;
	virtual void SetRtcAudioToSip(int callId, const char*pData, int nLen, int nSampleHz, int nChannel) = 0;
	
protected:
	SipProxy() {};
	virtual ~SipProxy() {};
};

#endif	// __SIP_CALL_H__
