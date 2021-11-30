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
#ifndef __SIP_RTC_MGR_H__
#define __SIP_RTC_MGR_H__
#include <list>
#include <map>
#include <string>
#include "SipCall.h"
#include "IArRtmCallManager.h"
#include "IArRtmService.h"
#include "SipCallToRtc.h"
#include "RtmToSip.h"
#include "SipToRtm.h"
#include "SipRtcStats.h"
#include "XAutoLock.h"

enum EventType
{
	ET_InitRtcSipCall = 0,
	ET_CloseRtcSipCall,
	ET_FreeRtcSipCall,
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

class SipRtcMgr : public SipProxyEvent, public ARM::IRtmServiceEventHandler, public ARM::IRtmCallEventHandler, public RtmToSipEvent, public SipToRtmEvent
{
public:
	SipRtcMgr(void);
	~SipRtcMgr(void);

	static SipRtcMgr&Inst();

	void SetRtcRtmAppId(const std::string&strAppId);
	void SetSipSvr(const std::string&strSipSvr);
	void SetRtcSvrPort(const std::string&strSvrIp, int nPort);
	void SetRtmSvrPort(const std::string&strSvrIp, int nPort);
	void SetPstnSvr(const std::string&strPstnSvr, const std::string&strPstnPrefix);

	void StartSipProxy(const std::string&strDomain, const std::string&strSipAccount, const std::string&strPwd, const std::string&strRtmAccount);
	void StartRtm2Sip(const std::string&strAccRule);
	void StopAll();

	bool ProcessMsg();

public:
	const std::string&RtcRtmAppId() { return str_rtc_rtm_app_id_; };
	const std::string&SipSvrAddr() { return str_sip_svr_ip_; };
	int SipSvrPort() { return n_sip_svr_port_; };
	const std::string&RtcSvrAddr() { return str_rtc_svr_ip_; };
	int RtcSvrPort() { return n_rtc_svr_port_; };
	const std::string&RtmSvrAddr() { return str_rtm_svr_ip_; };
	int RtmSvrPort() { return n_rtm_svr_port_; };

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
	virtual void onConnectionStateChanged(ARM::CONNECTION_STATE state, ARM::CONNECTION_CHANGE_REASON reason);
	virtual void onPeersOnlineStatusChanged(const ARM::PeerOnlineStatus peersStatus[], int peerCount);
	virtual void onMessageReceivedFromPeer(const char *peerId, const ARM::IMessage *message);

	//* For ARM::IRtmCallEventHandler
	virtual void onRemoteInvitationReceived(ARM::IRemoteCallInvitation *remoteInvitation);

	//* For RtmToSipEvent
	virtual void OnRtmToSipMakeCall(const std::string&strSessionId, const std::string&strCallerId, const std::string&strChanId, const std::string&strCallData, const std::string&strCallNumber, bool bPstn, RtmToSip*rtmToSip);
	virtual void OnRtmToSipEndCall(int nCallId);

	//* For SipToRtmEvent
	virtual void OnSipToRtmAcceptCall(int nCallId, const std::string&strCalleeId, SipToRtm*sipToRtm);
	virtual void OnSipToRtmEndCall(int nCallId);

private:
	//* Internal function
	void EndSipCallToRtc(const std::string&strCalleeId);
	void ReleaseSipCallToRtc(int callId, SipCallToRtc* rtcCallToSip);

	void ProcessMgrEvent();

private:
	SipRtcStats			ip_rtc_stats_;
	SipProxy			*sip_proxy_;
	ARM::IRtmService	*rtm_service_;
	ARM::IRtmCallManager*rtm_call_mgr_;
	int64_t				next_check_rtc_to_sip_time_;

	std::string			str_rtc_rtm_app_id_;
	std::string			str_rtc_svr_ip_;
	int					n_rtc_svr_port_;
	std::string			str_rtm_svr_ip_;
	int					n_rtm_svr_port_;

	std::string			str_pstn_svr_;
	std::string			str_pstn_prefix_;

private:
	//* 主线程事件队列
	XCritSec	cs_mgr_event_;
	std::list<MgrEvent*>	lst_mgr_event_;

private:
	std::string		str_sip_svr_ip_;
	int				n_sip_svr_port_;

private:
	typedef std::map<int/*callId*/, SipCallToRtc*> MapSipCallToRtc;
	XCritSec	cs_sip_call_to_rtc_;
	MapSipCallToRtc	map_sip_call_to_rtc_;

private:
	typedef std::map<std::string, RtmToSip*> MapRtmToSip;
	XCritSec	cs_rtm_to_sip_;
	MapRtmToSip	map_rtm_to_sip_;

	typedef std::map<std::string, SipToRtm*> MapSipToRtm;
	XCritSec	cs_sip_to_rtm_;
	MapSipToRtm	map_sip_to_rtm_;
};

#endif	// __SIP_RTC_MGR_H__