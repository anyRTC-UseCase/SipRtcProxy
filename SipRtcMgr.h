#ifndef __SIP_RTC_MGR_H__
#define __SIP_RTC_MGR_H__
#include <list>
#include <map>
#include <string>
#include "SipCall.h"
#include "IArRtmCallManager.h"
#include "IArRtmService.h"
#include "SipCallToRtc.h"
#include "RtcCallToSip.h"
#include "SipRtcStats.h"
#include "XAutoLock.h"

enum EventType
{
	ET_InitRtcAllToSip = 0,
	ET_CloseRtcAllToSip,
	ET_FreeRtcAllToSip,
	ET_CloseSipAllToRtc,
	ET_FreeSipAllToRtc,
};
typedef std::map<std::string, std::string>MapStr;
typedef std::map<std::string, int>MapInt;
struct MgrEvent
{
	MgrEvent(void) :ptr(NULL){}
	EventType	eType;
	MapStr	mapStr;
	MapInt	mapInt;
	void* ptr;
};

class SipRtcMgr : public SipProxyEvent, public ARM::IRtmServiceEventHandler, public ARM::IRtmCallEventHandler, public RtcCallToSipEvent
{
public:
	SipRtcMgr(void);
	~SipRtcMgr(void);

	void SetRtcRtmAppId(const std::string&strAppId);
	void SetRtcSvrPort(const std::string&strSvrIp, int nPort);
	void SetRtmSvrPort(const std::string&strSvrIp, int nPort);
	bool SetSipAccount(const std::string&strSvrIp, int nPort, const std::string&strPrefix, const std::string&strRule, const std::string&strPwd);
	
	void StartSipProxy(const std::string&strDomain, const std::string&strSipAccount, const std::string&strPwd);
	void StartIvr(const std::string&strRtmAccount, const std::string&strSipAccount);
	void StopAll();

	bool ProcessMsg();

public:
	//* For SipProxyEvent
	// 遠程Sip呼叫，需要判斷是否需要進行雙呼(即呼叫App也呼叫內外Sip座機)
	virtual void OnSipIncomingCall(int callId, const std::string&strFromSipId, const std::string&strToSipId, int &nDualCall, const std::string&strCusData);
	// 直接呼叫App
	virtual void OnSipCallApp(int callId, const std::string&strFromSipId, const std::string&strToSipId, const std::string&strCusData);
	virtual void OnSipEndCall(int callId);
	virtual void OnSipNeedPstn(int callId);
	virtual void OnSipReleasePstn(const std::string&strFxoAccount);
	virtual void OnRecvSipPeerAudio(int callId, const char*pData, int nLen, int nSampleHz, int nChannel);

	//* For  ARM::IRtmServiceEventHandler
	virtual void onLoginSuccess();
	virtual void onLoginFailure(ARM::LOGIN_ERR_CODE errorCode);
	virtual void onLogout(ARM::LOGOUT_ERR_CODE errorCode);
	virtual void onPeersOnlineStatusChanged(const ARM::PeerOnlineStatus peersStatus[], int peerCount);
	virtual void onMessageReceivedFromPeer(const char *peerId, const ARM::IMessage *message);

	//* For ARM::IRtmCallEventHandler
	virtual void onLocalInvitationReceivedByPeer(ARM::ILocalCallInvitation *localInvitation);
	virtual void onLocalInvitationCanceled(ARM::ILocalCallInvitation *localInvitation);
	virtual void onLocalInvitationFailure(ARM::ILocalCallInvitation *localInvitation, ARM::LOCAL_INVITATION_ERR_CODE errorCode);
	virtual void onLocalInvitationAccepted(ARM::ILocalCallInvitation *localInvitation, const char *response);
	virtual void onLocalInvitationRefused(ARM::ILocalCallInvitation *localInvitation, const char *response);
	virtual void onRemoteInvitationRefused(ARM::IRemoteCallInvitation *remoteInvitation);
	virtual void onRemoteInvitationAccepted(ARM::IRemoteCallInvitation *remoteInvitation);
	virtual void onRemoteInvitationReceived(ARM::IRemoteCallInvitation *remoteInvitation);
	virtual void onRemoteInvitationFailure(ARM::IRemoteCallInvitation *remoteInvitation, ARM::REMOTE_INVITATION_ERR_CODE errorCode);
	virtual void onRemoteInvitationCanceled(ARM::IRemoteCallInvitation *remoteInvitation);

	//* For RtcCallToSipEvent
	virtual void OnRtcCallToSipClosed(const std::string&strCallerId, int nCode);

private:
	//* Internal function
	const std::string&AllocSipAccount();
	void FreeSipAccount(const std::string&strSipAccount);
	void EndSipCallToRtc(const std::string&strCalleeId);
	void ReleaseSipCallToRtc(int callId, SipCallToRtc* rtcCallToSip);

	void EndRtcCallToSip(const std::string&strCallerId);
	void InitRtcCallToSip(const std::string&strCallerId, const std::string&strChanId, const std::string&strSipAccount, const std::string&strSipData);
	void ReleaseRtcCallToSip(RtcCallToSip* rtcCallToSip);

	void ProcessMgrEvent();

private:
	SipRtcStats			ip_rtc_stats_;
	SipProxy			*sip_proxy_;
	ARM::IRtmService	*rtm_service_;
	ARM::IRtmCallManager*rtm_call_mgr_;

	std::string			str_rtc_rtm_app_id_;
	std::string			str_rtc_svr_ip_;
	int					n_rtc_svr_port_;
	std::string			str_rtm_svr_ip_;
	int					n_rtm_svr_port_;

	std::string			str_ivr_sip_account_;

private:
	XCritSec	cs_mgr_event_;
	std::list< MgrEvent*>	lst_mgr_event_;

private:
	typedef std::map<std::string, int> MapSipAccount;
	XCritSec	cs_sip_account_;
	MapSipAccount	map_sip_account_;
	std::string		str_sip_password_;
	std::string		str_sip_svr_ip_;
	int				n_sip_svr_port_;

private:
	typedef std::map<std::string/*uid*/, RtcCallToSip*> MapRtcCallToSip;
	XCritSec	cs_rtc_call_to_sip_;
	MapRtcCallToSip	map_rtc_call_to_sip_;

	typedef std::map<int/*callId*/, SipCallToRtc*> MapSipCallToRtc;
	XCritSec	cs_sip_call_to_rtc_;
	MapSipCallToRtc	map_sip_call_to_rtc_;

};

#endif	// __SIP_RTC_MGR_H__