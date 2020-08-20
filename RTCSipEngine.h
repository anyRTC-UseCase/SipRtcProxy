#ifndef __RTC_SIP_ENGINE_H__
#define __RTC_SIP_ENGINE_H__
#include <string>
#include "LIV_Export.h"

/*
1, RTCSipEngine 入会作为Monitor对会议进行监控，并订阅所有的RTC流(除了SipHelper)
2, RTCSipHelper 作为Siper入会(所有来自服务的Subscribe通知都不管)，主要作用是发布一路Sip->Rtc的流，同时将混合音频发给sip
*/

class LIV_API RTCSipEngine
{
public:
	static void Init(const std::string&strSipSvrIp, int nSipSvrPort);
	static void Deinit();

	

	static void ConfigServerForPriCloud(const char*strSvrAddr, int nSvrPort);
	static void SetLogLevel(int nLevel);
	static bool SetSipAccount(const char*strSvrIp, int nPort, const char*strPrefix, const char*strRule, const char*strPassword);
	static bool SetSipFxoAccount(const char*strPrefix, const char*strCount, const char*strAddr);
	static void SetLocalPublicIPAddr(const char*strLocalAddr, const char*strPublicAddr);
	static bool SetPortAllocatoer(int nPortMin, int nPortMax);

public:
	virtual int MakeCall(const std::string&strPhoneNumber, bool bPstn, bool bPrefix, const std::string&strUserCustomData) = 0;
	virtual void EndCall(const std::string&strPhoneNumber) = 0;

protected:
	RTCSipEngine() {};
	virtual ~RTCSipEngine() {};
};

class LIV_API RTCSipMeetEngine : public RTCSipEngine
{
public:
	static RTCSipMeetEngine*Create();
	static void Destory(RTCSipMeetEngine*pEngine);

	virtual void StartMeet(const std::string&strAppID, const std::string&strAnyrtcID, int meetMode) = 0;
	virtual void StopMeet() = 0;

protected:
	RTCSipMeetEngine(void) {};
	virtual~RTCSipMeetEngine(void) {};
};

class RTCSipP2PEngineEvent {
public:
	RTCSipP2PEngineEvent(void) {};
	virtual ~RTCSipP2PEngineEvent(void) {};

	virtual void OnRtcAccept(int callId) = 0;
	virtual void OnRecvRtcPeerAudio(int callId, const char*pData, int nLen, int nSampleHz, int nChannel) = 0;
};

class LIV_API RTCSipP2PEngine : public RTCSipEngine
{
public:
	static RTCSipP2PEngine*Create();
	static void Destory(RTCSipP2PEngine*pEngine);

	virtual void StartP2P(const std::string&strAppID, const std::string&strAnyrtcID, const std::string&strUserId) = 0;
	virtual void StopP2P() = 0;
	virtual bool Released() = 0;

	virtual void SetSipRtcEvent(int callId, RTCSipP2PEngineEvent*pEvent, const std::string&strPeerUId, const std::string&strPeerUData) = 0;
	virtual void SetSipAudioToRtc(const char*pData, int nLen, int nSampleHz, int nChannel) = 0;

protected:
	RTCSipP2PEngine(void) {};
	virtual~RTCSipP2PEngine(void) {};
};

#endif	// __RTC_SIP_ENGINE_H__