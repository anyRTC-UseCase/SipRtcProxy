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
#include "SipRtcMgr.h"
#include "RtcLog.h"
#include <assert.h>
#include "rapidjson/json_value.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"	
#include "rapidjson/stringbuffer.h"

enum CallType
{
	RtmCallToSip = 0,
	SipCallToRtm,
};

size_t str_split(const std::string& source,
	char delimiter,
	std::vector<std::string>* fields) {
	fields->clear();
	size_t last = 0;
	for (size_t i = 0; i < source.length(); ++i) {
		if (source[i] == delimiter) {
			fields->push_back(source.substr(last, i - last));
			last = i + 1;
		}
	}
	fields->push_back(source.substr(last, source.length() - last));
	return fields->size();
}

#define NullStr ""
#define TIMER_1S 1000
static SipRtcMgr* gInst = NULL;

SipRtcMgr&SipRtcMgr::Inst()
{
	return *gInst;
}

SipRtcMgr::SipRtcMgr(void)
	: sip_proxy_(NULL)
	, rtm_service_(NULL)
	, rtm_call_mgr_(NULL)
	, next_check_rtc_to_sip_time_(0)
	, n_rtc_svr_port_(0)
	, n_rtm_svr_port_(0)
	, n_sip_svr_port_(5060)
{
	gInst = this;
}
SipRtcMgr::~SipRtcMgr(void)
{
	gInst = NULL;
	assert(sip_proxy_ == NULL);
	assert(rtm_service_ == NULL);
	assert(rtm_call_mgr_ == NULL);
}

void SipRtcMgr::SetRtcRtmAppId(const std::string&strAppId)
{
	str_rtc_rtm_app_id_ = strAppId;
}
void SipRtcMgr::SetSipSvr(const std::string&strSipSvr)
{
	std::string strSipSvrIp;
	int nSipPort = 5060;
	size_t pos = strSipSvr.find(":");
	if (pos != std::string::npos) {
		strSipSvrIp = strSipSvr.substr(0, pos);
		std::string strPort = strSipSvr.substr(pos + 1);
		nSipPort = atoi(strPort.c_str());
	}
	else {
		strSipSvrIp = strSipSvr;
	}
	str_sip_svr_ip_ = strSipSvrIp;
	n_sip_svr_port_ = nSipPort;
}
void SipRtcMgr::SetRtcSvrPort(const std::string&strSvrIp, int nPort)
{
	str_rtc_svr_ip_ = strSvrIp;
	n_rtc_svr_port_ = nPort;
}
void SipRtcMgr::SetRtmSvrPort(const std::string&strSvrIp, int nPort)
{
	str_rtm_svr_ip_ = strSvrIp;
	n_rtm_svr_port_ = nPort;
}
void SipRtcMgr::SetPstnSvr(const std::string&strPstnSvr, const std::string&strPstnPrefix)
{
	str_pstn_svr_ = strPstnSvr;
	str_pstn_prefix_ = strPstnPrefix;
}

void SipRtcMgr::StartSipProxy(const std::string&strDomain, const std::string&strSipAccount, const std::string&strPwd, const std::string&strRtmAccount)
{
	if (sip_proxy_ == NULL) {
		sip_proxy_ = SipProxy::Create(*this, strDomain, strSipAccount, strPwd);
	}

	if (rtm_service_ == NULL) {
		rtm_service_ = ARM::createRtmService();
		rtm_service_->initialize(str_rtc_rtm_app_id_.c_str(), this);
		//rtm_service_->setLogFile("./sip_rtm.log");
		if (str_rtm_svr_ip_.length() > 0 && n_rtm_svr_port_ > 0) {
			char strParams[1024];
			sprintf(strParams, "{\"Cmd\":\"ConfPriCloudAddr\", \"ServerAdd\": \"%s\", \"Port\": %d}", str_rtm_svr_ip_.c_str(), n_rtm_svr_port_);
			rtm_service_->setParameters(strParams);
		}
		int ret = rtm_service_->login(NULL, strRtmAccount.c_str());
		printf("RtmService login: %d\r\n", ret);

		rtm_call_mgr_ = rtm_service_->getRtmCallManager(this);
	}
}

void SipRtcMgr::StartRtm2Sip(const std::string&strAccRule)
{//1016;20[01-99]
	std::vector<std::string> arrAccount;
	str_split(strAccRule, ';', &arrAccount);
	if(arrAccount.size() > 0) {
		for (int i = 0; i < arrAccount.size(); i++) {
			const std::string&strAccount = arrAccount[i];
			int nFind = strAccount.find("[");
			if (nFind != std::string::npos && strAccount.c_str()[strAccount.length() -1] == ']') {
				std::string strPrefix;
				std::string strFrom;
				std::string strTo;
				strPrefix = strAccount.substr(0, nFind);
				std::string strRuleCopy = strAccount.substr(nFind + 1, strAccount.length() - (nFind+1) - 1);
				size_t pos = strRuleCopy.find("-");
				if (pos != std::string::npos) {
					strFrom = strRuleCopy.substr(0, pos);
					strTo = strRuleCopy.substr(pos + 1);

					int nMinDigit = strFrom.length();
					int nFrom = atoi(strFrom.c_str());
					int nTo = atoi(strTo.c_str());
					if (nTo > nFrom) {
						char *pAccount = new char[strlen(strPrefix.c_str()) + nTo + 16];
						for (int i = nFrom; i <= nTo; i++) {
							sprintf(pAccount, "%s%.*d", strPrefix.c_str(), nMinDigit, i);
							XAutoLock l(cs_rtm_to_sip_);
							if (map_rtm_to_sip_.find(pAccount) == map_rtm_to_sip_.end()) {
								RtmToSip*rtmToSip = new RtmToSip(*this);
								rtmToSip->StartRtm(pAccount);
								map_rtm_to_sip_[pAccount] = rtmToSip;
							}
						}
						delete[] pAccount;
					}
				}
			}
			else {
				XAutoLock l(cs_rtm_to_sip_);
				if (map_rtm_to_sip_.find(strAccount) == map_rtm_to_sip_.end()) {
					RtmToSip*rtmToSip = new RtmToSip(*this);
					rtmToSip->StartRtm(strAccount);
					map_rtm_to_sip_[strAccount] = rtmToSip;
				}
			}
		}
	}
}

void SipRtcMgr::StopAll()
{
	XAutoLock l(cs_rtm_to_sip_);
	while (map_rtm_to_sip_.size() > 0) {
		MapRtmToSip::iterator itrr = map_rtm_to_sip_.begin();
		RtmToSip*rtmToSip = itrr->second;
		map_rtm_to_sip_.erase(itrr);
		rtmToSip->StopRtm();
		delete rtmToSip;
		rtmToSip = NULL;
	}

	if (rtm_service_ != NULL) {
		rtm_call_mgr_->release();
		rtm_call_mgr_ = NULL;

		rtm_service_->logout();
		rtm_service_->release();
		rtm_service_ = NULL;
	}

	if (sip_proxy_ != NULL) {
		SipProxy::Destory(sip_proxy_);
		sip_proxy_ = NULL;
	}
}

bool SipRtcMgr::ProcessMsg()
{
	ProcessMgrEvent();

	if(next_check_rtc_to_sip_time_<=XGetUtcTimestamp())
	{
		next_check_rtc_to_sip_time_ = XGetUtcTimestamp() + TIMER_1S;

		XAutoLock l(cs_rtm_to_sip_);
		MapRtmToSip::iterator iter = map_rtm_to_sip_.begin();
		while (iter != map_rtm_to_sip_.end()) {
			iter->second->ProcessMsg();
			iter++;
		}
	}
	return true;
}


//* For SipProxyEvent
void SipRtcMgr::OnSipIncomingCall(int callId, const std::string&strFromSipId, const std::string&strToSipId, int &nDualCall, const std::string&strCusData) 
{
	XAutoLock l(cs_sip_call_to_rtc_);
	if (map_sip_call_to_rtc_.find(callId) == map_sip_call_to_rtc_.end()) {
		std::string strChanId ;
		XGetRandomStr(strChanId, 9);
		std::string strCallerId;
		XGetRandomStr(strCallerId, 4);

		SipCallToRtc* sipCallToRtc = new SipCallToRtc(callId, sip_proxy_);
		sipCallToRtc->SetCalleeId(strToSipId);
		sipCallToRtc->SetServer(str_rtc_svr_ip_, n_rtc_svr_port_);
		sipCallToRtc->StartTask(str_rtc_rtm_app_id_, strChanId, strCallerId);

		SipToRtm*sipToRtm = new SipToRtm(*this);
		sipToRtm->SetSipCallInfo(callId, strChanId, strFromSipId, strToSipId, strCusData);
		sipToRtm->StartRtm(strCallerId);

		sipCallToRtc->SetUserData(SipCallToRtm, sipToRtm);
		map_sip_call_to_rtc_[callId] = sipCallToRtc;

		{// Subscribe peer online status
			long long reqId = 0;
			const char* peerIds[1];
			peerIds[0] = strToSipId.c_str();
			rtm_service_->subscribePeersOnlineStatus(peerIds, 1, reqId);
		}
	}
}
void SipRtcMgr::OnSipCallApp(int callId, const std::string&strFromSipId, const std::string&strToSipId, const std::string&strCusData)
{
}
void SipRtcMgr::OnSipEndCall(int callId) 
{
	{
		XAutoLock l(cs_sip_call_to_rtc_);
		if (map_sip_call_to_rtc_.find(callId) != map_sip_call_to_rtc_.end()) {
			SipCallToRtc* sipCallToRtc = map_sip_call_to_rtc_[callId];
			sipCallToRtc->StopTask();
			if (sipCallToRtc->GetUserType() == RtmCallToSip) {
				RtmToSip*rtmToSip = (RtmToSip*)sipCallToRtc->GetUserData();
				rtmToSip->SipEndCall();
			}
			else {
				SipToRtm*sipToRtm = (SipToRtm*)sipCallToRtc->GetUserData();
				sipToRtm->SipEndCall();
			}
		}
	}

	MgrEvent* mgrEvent = new MgrEvent();
	mgrEvent->eType = ET_CloseRtcSipCall;
	mgrEvent->mapInt["CallId"] = callId;

	XAutoLock l(cs_mgr_event_);
	lst_mgr_event_.push_back(mgrEvent);
}
void SipRtcMgr::OnSipNeedPstn(int callId) 
{
}
void SipRtcMgr::OnSipReleasePstn(const std::string&strFxoAccount) 
{
}

void SipRtcMgr::OnRecvSipPeerAudio(int callId, const char*pData, int nLen, int nSampleHz, int nChannels)
{
	XAutoLock l(cs_sip_call_to_rtc_);
	if (map_sip_call_to_rtc_.find(callId) != map_sip_call_to_rtc_.end()) {
		SipCallToRtc* sipCallToRtc = map_sip_call_to_rtc_[callId];

		sipCallToRtc->SetSipAudToRtc(pData, nLen, nSampleHz, nChannels);
	}
}

//* For  ARM::IRtmServiceEventHandler
void SipRtcMgr::onLoginSuccess()
{
	printf("[RTM] onLoginSuccess \r\n");
}
void SipRtcMgr::onLoginFailure(ARM::LOGIN_ERR_CODE errorCode)
{
	printf("[RTM] onLoginFailure : %d \r\n", errorCode);
}
void SipRtcMgr::onLogout(ARM::LOGOUT_ERR_CODE errorCode)
{
	printf("[RTM] onLogout : %d \r\n", errorCode);
}
void SipRtcMgr::onConnectionStateChanged(ARM::CONNECTION_STATE state, ARM::CONNECTION_CHANGE_REASON reason)
{
	printf("[RTM] onConnectionStateChanged state: %d reason: %d\r\n", state, reason);
}
void SipRtcMgr::onPeersOnlineStatusChanged(const ARM::PeerOnlineStatus peersStatus[], int peerCount)
{
	for (int i = 0; i < peerCount; i++) {
		const ARM::PeerOnlineStatus&peerStats = peersStatus[i];
		if (!peerStats.isOnline) {
			EndSipCallToRtc(peerStats.peerId);
		}
	}
}
void SipRtcMgr::onMessageReceivedFromPeer(const char *peerId, const ARM::IMessage *message)
{
	rapidjson::Document		jsonReqDoc;
	JsonStr sprCopy(message->getText(), strlen(message->getText()));
	if (!jsonReqDoc.ParseInsitu<0>(sprCopy.Ptr).HasParseError()) {
		const char*strCmd = GetJsonStr(jsonReqDoc, "Cmd", F_AT);
		if (strcmp("EndCall", strCmd) == 0) {
			// 对方挂断
			EndSipCallToRtc(peerId);
		}
	}
}

//* For ARM::IRtmCallEventHandler
// 对方发起呼叫
void SipRtcMgr::onRemoteInvitationReceived(ARM::IRemoteCallInvitation *remoteInvitation)
{// Proxy禁止Rtm发起呼叫
	remoteInvitation->setResponse("joined");
	rtm_call_mgr_->refuseRemoteInvitation(remoteInvitation);
}

//* For RtmToSipEvent
void SipRtcMgr::OnRtmToSipMakeCall(const std::string&strSessionId, const std::string&strCallerId, const std::string&strChanId, const std::string&strCallData, const std::string&strCallNumber, bool bPstn, RtmToSip*rtmToSip)
{
	MgrEvent* mgrEvent = new MgrEvent();
	mgrEvent->eType = ET_InitRtcSipCall;
	mgrEvent->mapStr["SessionId"] = strSessionId;
	mgrEvent->mapStr["CallerId"] = strCallerId;
	mgrEvent->mapStr["ChanId"] = strChanId;
	mgrEvent->mapStr["CallData"] = strCallData;
	mgrEvent->mapStr["CallNumber"] = strCallNumber;
	mgrEvent->mapInt["Pstn"] = bPstn ? 1 : 0;
	mgrEvent->ptr = rtmToSip;

	XAutoLock l(cs_mgr_event_);
	lst_mgr_event_.push_back(mgrEvent);
}
void SipRtcMgr::OnRtmToSipEndCall(int nCallId)
{
	MgrEvent* mgrEvent = new MgrEvent();
	mgrEvent->eType = ET_CloseRtcSipCall;
	mgrEvent->mapInt["CallId"] = nCallId;

	XAutoLock l(cs_mgr_event_);
	lst_mgr_event_.push_back(mgrEvent);
}

//* For SipToRtmEvent
void SipRtcMgr::OnSipToRtmAcceptCall(int nCallId, const std::string&strCalleeId, SipToRtm*sipToRtm)
{

	XAutoLock l(cs_sip_call_to_rtc_);
	MapSipCallToRtc::iterator itsr = map_sip_call_to_rtc_.find(nCallId);
	if (itsr != map_sip_call_to_rtc_.end()) {
		sip_proxy_->RtcAcceptCall(nCallId);
	}
}
void SipRtcMgr::OnSipToRtmEndCall(int nCallId)
{
	{
		XAutoLock l(cs_sip_call_to_rtc_);
		MapSipCallToRtc::iterator itsr = map_sip_call_to_rtc_.find(nCallId);
		if (itsr != map_sip_call_to_rtc_.end()) {
			ReleaseSipCallToRtc(itsr->first, itsr->second);

			long long reqId = 0;
			const char* peerIds[1];
			peerIds[0] = itsr->second->CalleeId().c_str();
			rtm_service_->unsubscribePeersOnlineStatus(peerIds, 1, reqId);
			map_sip_call_to_rtc_.erase(itsr);
		}
	}
}

void SipRtcMgr::EndSipCallToRtc(const std::string&strCalleeId)
{
	{
		XAutoLock l(cs_sip_call_to_rtc_);
		MapSipCallToRtc::iterator itsr = map_sip_call_to_rtc_.begin();
		while (itsr != map_sip_call_to_rtc_.end()) {
			if (itsr->second->CalleeId().compare(strCalleeId) == 0) {
				ReleaseSipCallToRtc(itsr->first, itsr->second);

				long long reqId = 0;
				const char* peerIds[1];
				peerIds[0] = itsr->second->CalleeId().c_str();
				rtm_service_->unsubscribePeersOnlineStatus(peerIds, 1, reqId);
				map_sip_call_to_rtc_.erase(itsr);
				break;
			}
			itsr++;
		}
	}
}
void SipRtcMgr::ReleaseSipCallToRtc(int callId, SipCallToRtc* rtcCallToSip)
{// 线程同步
	MgrEvent* mgrEvent = new MgrEvent();
	mgrEvent->eType = ET_FreeRtcSipCall;
	mgrEvent->ptr = rtcCallToSip;
	mgrEvent->mapInt["CallId"] = callId;

	XAutoLock l(cs_mgr_event_);
	lst_mgr_event_.push_back(mgrEvent);
}

void SipRtcMgr::ProcessMgrEvent()
{
	MgrEvent* mgrEvent = NULL;
	{
		XAutoLock l(cs_mgr_event_);
		if (lst_mgr_event_.size() > 0) {
			mgrEvent = lst_mgr_event_.front();
			lst_mgr_event_.pop_front();
		}
	}

	if (mgrEvent != NULL) {
		if (mgrEvent->eType == ET_InitRtcSipCall) {
			const std::string&strSessionId = mgrEvent->mapStr["SessionId"];
			const std::string&strCallerId = mgrEvent->mapStr["CallerId"];
			const std::string&strChanId = mgrEvent->mapStr["ChanId"];
			const std::string&strCallData = mgrEvent->mapStr["CallData"];
			const std::string&strCallNumber = mgrEvent->mapStr["CallNumber"];
			bool bPstn = false;
			if (mgrEvent->mapInt.find("Pstn") != mgrEvent->mapInt.end()) {
				bPstn = mgrEvent->mapInt["Pstn"] == 1;
			}
			RtmToSip*rtmToSip = (RtmToSip*)mgrEvent->ptr;
			char strUrl[256];
			if (bPstn) {// Pstn需要将账号换为CallNumber
				if (str_pstn_svr_.length() > 0) {
					sprintf(strUrl, "sip:%s%s@%s", str_pstn_prefix_.c_str(), strCallNumber.c_str(), str_pstn_svr_.c_str());
				}
				else {
					sprintf(strUrl, "sip:%s%s@%s", str_pstn_prefix_.c_str(), strCallNumber.c_str(), SipSvrAddr().c_str());
				}
			}
			else {
				sprintf(strUrl, "sip:%s@%s", rtmToSip->RtmAccount().c_str(), SipSvrAddr().c_str());
			}
			
			//DisplayName for sip: https://blog.csdn.net/gredn/article/details/109214785
			int nCallId = sip_proxy_->RtcMakeCall(strUrl, strCallNumber.c_str(), strCallData, false);
			if (nCallId != -1) {
				rtmToSip->SetSipCallId(strSessionId, nCallId);

				{
					XAutoLock l(cs_sip_call_to_rtc_);
					if (map_sip_call_to_rtc_.find(nCallId) == map_sip_call_to_rtc_.end()) {

						SipCallToRtc* sipCallToRtc = new SipCallToRtc(nCallId, sip_proxy_);
						sipCallToRtc->SetCalleeId(strCallerId);
						sipCallToRtc->SetServer(str_rtc_svr_ip_, n_rtc_svr_port_);
						sipCallToRtc->SetUserData(RtmCallToSip, rtmToSip);
						sipCallToRtc->StartTask(str_rtc_rtm_app_id_, strChanId, rtmToSip->RtmAccount());

						map_sip_call_to_rtc_[nCallId] = sipCallToRtc;
					}
				}

				{// Subscribe peer online status
					long long reqId = 0;
					const char* peerIds[1];
					peerIds[0] = strCallerId.c_str();
					rtm_service_->subscribePeersOnlineStatus(peerIds, 1, reqId);
				}
			}
			else {
				rtmToSip->SipEndCall();
			}
		}
		else if (mgrEvent->eType == ET_CloseRtcSipCall) {
			int nCallId = mgrEvent->mapInt["CallId"];
			std::string strCalleeId;
			{
				XAutoLock l(cs_sip_call_to_rtc_);
				if (map_sip_call_to_rtc_.find(nCallId) != map_sip_call_to_rtc_.end()) {
					SipCallToRtc* sipCallToRtc = map_sip_call_to_rtc_[nCallId];
					strCalleeId = sipCallToRtc->CalleeId();		
				}
			}
			if (strCalleeId.length() > 0) {
				EndSipCallToRtc(strCalleeId);
			}
		}
		else if (mgrEvent->eType == ET_FreeRtcSipCall) {
			SipCallToRtc*sipCallToRtc = (SipCallToRtc*)mgrEvent->ptr;
			sipCallToRtc->StopTask();
			if (sipCallToRtc->GetUserType() == SipCallToRtm) {
				SipToRtm* sipToRtm = (SipToRtm*)sipCallToRtc->GetUserData();
				sipToRtm->StopRtm();
				delete sipToRtm;
				sipToRtm = NULL;
			}
			else if (sipCallToRtc->GetUserType() == RtmCallToSip) {
				RtmToSip*rtmToSip = (RtmToSip*)sipCallToRtc->GetUserData();
				rtmToSip->SipEndCall();
				//* 不能在這裡刪除，RtmToSip有自己管理的地方
				/*rtmToSip->StopRtm();
				delete rtmToSip;
				rtmToSip = NULL;*/
			}
			
			delete sipCallToRtc;
			sipCallToRtc = NULL;

			int nCallId = mgrEvent->mapInt["CallId"];
			sip_proxy_->RtcEndCall(nCallId);
		}

		delete mgrEvent;
		mgrEvent = NULL;
	}
}