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

#define NullStr ""

SipRtcMgr::SipRtcMgr(void)
	: sip_proxy_(NULL)
	, rtm_service_(NULL)
	, rtm_call_mgr_(NULL)
	, n_rtc_svr_port_(0)
	, n_rtm_svr_port_(0)
	, n_sip_svr_port_(5060)
{

}
SipRtcMgr::~SipRtcMgr(void)
{
	assert(sip_proxy_ == NULL);
	assert(rtm_service_ == NULL);
	assert(rtm_call_mgr_ == NULL);
}

void SipRtcMgr::SetRtcRtmAppId(const std::string&strAppId)
{
	str_rtc_rtm_app_id_ = strAppId;
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
bool SipRtcMgr::SetSipAccount(const std::string&strSvrIp, int nPort, const std::string&strPrefix, const std::string&strRule, const std::string&strPassword)
{
	if (strRule.length() == 0) {
		//RtcLog(ERR, "Sip account rule is null");
		return false;
	}
	std::string strFrom;
	std::string strTo;
	std::string strRuleCopy = strRule;
	size_t pos = strRuleCopy.find("-");
	if (pos != std::string::npos) {
		strFrom = strRuleCopy.substr(0, pos);
		strTo = strRuleCopy.substr(pos + 1);
	}
	if (strFrom.length() == 0 || strTo.length() == 0) {
		//RtcLog(ERR, "Sip account rule need xxx-yyy");
		return false;
	}
	if (strFrom.length() > strTo.length()) {
		//RtcLog(ERR, "Sip account rule(xxx-yyy) is error, length of xxx need small than yyy");
		return false;
	}
	int nMinDigit = strFrom.length();
	int nFrom = atoi(strFrom.c_str());
	int nTo = atoi(strTo.c_str());
	if (nTo < nFrom) {
		//RtcLog(ERR, "Sip account rule(xxx-yyy) is error, yyy need bigger than xxx");
		return false;
	}
	str_sip_svr_ip_ = strSvrIp;
	n_sip_svr_port_ = nPort;
	str_sip_password_ = strPassword;
	char *pAccount = new char[strlen(strPrefix.c_str()) + nTo + 16];
	for (int i = nFrom; i <= nTo; i++) {
		sprintf(pAccount, "%s%.*d", strPrefix.c_str(), nMinDigit, i);
		XAutoLock l(cs_sip_account_);
		map_sip_account_[pAccount] = 0;
	}
	delete[] pAccount;

	return true;
}
void SipRtcMgr::StartSipProxy(const std::string&strDomain, const std::string&strSipAccount, const std::string&strPwd)
{
	if (sip_proxy_ == NULL) {
		sip_proxy_ = SipProxy::Create(*this, strDomain, strSipAccount, strPwd);
	}
}

void SipRtcMgr::StartIvr(const std::string&strRtmAccount, const std::string&strSipAccount)
{
	if (rtm_service_ == NULL) {
		rtm_service_ = ARM::createRtmService();
		rtm_service_->initialize(str_rtc_rtm_app_id_.c_str(), this);
		rtm_service_->setLogFile("./sip_rtm.log");
		rtm_service_->login(NULL, strRtmAccount.c_str());

		rtm_call_mgr_ = rtm_service_->getRtmCallManager(this);

		str_ivr_sip_account_ = strSipAccount;
	}
}
void SipRtcMgr::StopAll()
{
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
	return true;
}


//* For SipProxyEvent
void SipRtcMgr::OnSipIncomingCall(int callId, const std::string&strFromSipId, const std::string&strToSipId, int &nDualCall, const std::string&strCusData) 
{
	XAutoLock l(cs_sip_call_to_rtc_);
	if (map_sip_call_to_rtc_.find(callId) == map_sip_call_to_rtc_.end()) {
		SipCallToRtc* sipCallToRtc = new SipCallToRtc(callId, sip_proxy_);
		sipCallToRtc->SetCalleeId(strToSipId);
		sipCallToRtc->StartTask(str_rtc_rtm_app_id_, strFromSipId, strFromSipId);

		ARM::ILocalCallInvitation* localInv = rtm_call_mgr_->createLocalCallInvitation(strToSipId.c_str());
		rapidjson::Document		jsonDoc;
		rapidjson::StringBuffer jsonStr;
		rapidjson::Writer<rapidjson::StringBuffer> jsonWriter(jsonStr);
		jsonDoc.SetObject();
		jsonDoc.AddMember("Mode", 1, jsonDoc.GetAllocator());
		jsonDoc.AddMember("Conference", 0, jsonDoc.GetAllocator());
		jsonDoc.AddMember("ChanId", strFromSipId.c_str(), jsonDoc.GetAllocator());
		jsonDoc.AddMember("SipData", strCusData.c_str(), jsonDoc.GetAllocator());
		jsonDoc.Accept(jsonWriter);
		localInv->setContent(jsonStr.GetString());
		rtm_call_mgr_->sendLocalInvitation(localInv);

		map_sip_call_to_rtc_[callId] = sipCallToRtc;

		long long reqId = 0;
		const char* peerIds[1];
		peerIds[0] = strToSipId.c_str();
		rtm_service_->subscribePeersOnlineStatus(peerIds, 1, reqId);
	}
}
void SipRtcMgr::OnSipCallApp(int callId, const std::string&strFromSipId, const std::string&strToSipId, const std::string&strCusData)
{
}
void SipRtcMgr::OnSipEndCall(int callId) 
{
	MgrEvent* mgrEvent = new MgrEvent();
	mgrEvent->eType = ET_CloseSipAllToRtc;
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
}
void SipRtcMgr::onLoginFailure(ARM::LOGIN_ERR_CODE errorCode)
{
}
void SipRtcMgr::onLogout(ARM::LOGOUT_ERR_CODE errorCode)
{
}
void SipRtcMgr::onPeersOnlineStatusChanged(const ARM::PeerOnlineStatus peersStatus[], int peerCount)
{
	for (int i = 0; i < peerCount; i++) {
		const ARM::PeerOnlineStatus&peerStats = peersStatus[i];
		if (!peerStats.isOnline) {
			EndRtcCallToSip(peerStats.peerId);
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
			EndRtcCallToSip(peerId);	
			EndSipCallToRtc(peerId);
		}
	}
}

//* For ARM::IRtmCallEventHandler
void SipRtcMgr::onLocalInvitationReceivedByPeer(ARM::ILocalCallInvitation *localInvitation)
{
}
void SipRtcMgr::onLocalInvitationCanceled(ARM::ILocalCallInvitation *localInvitation)
{// Sip呼叫Rtc，本地取消成功
	EndSipCallToRtc(localInvitation->getCalleeId());
}
void SipRtcMgr::onLocalInvitationFailure(ARM::ILocalCallInvitation *localInvitation, ARM::LOCAL_INVITATION_ERR_CODE errorCode)
{// Sip呼叫Rtc失败了
	EndSipCallToRtc(localInvitation->getCalleeId());
}
void SipRtcMgr::onLocalInvitationAccepted(ARM::ILocalCallInvitation *localInvitation, const char *response)
{// Sip呼叫Rtc，对方接听
	XAutoLock l(cs_sip_call_to_rtc_);
	MapSipCallToRtc::iterator itsr = map_sip_call_to_rtc_.begin();
	while (itsr != map_sip_call_to_rtc_.end()) {
		if (itsr->second->CalleeId().compare(localInvitation->getCalleeId()) == 0) {
			sip_proxy_->RtcAcceptCall(itsr->first);
			break;
		}
		itsr++;
	}
}
void SipRtcMgr::onLocalInvitationRefused(ARM::ILocalCallInvitation *localInvitation, const char *response)
{// Sip呼叫Rtc，对方拒绝
	EndSipCallToRtc(localInvitation->getCalleeId());
}
void SipRtcMgr::onRemoteInvitationRefused(ARM::IRemoteCallInvitation *remoteInvitation)
{//拒绝对方邀请成功
}
void SipRtcMgr::onRemoteInvitationAccepted(ARM::IRemoteCallInvitation *remoteInvitation){}
// 对方发起呼叫
void SipRtcMgr::onRemoteInvitationReceived(ARM::IRemoteCallInvitation *remoteInvitation)
{
	rapidjson::Document		jsonReqDoc;
	JsonStr sprCopy(remoteInvitation->getContent(), strlen(remoteInvitation->getContent()));
	if (!jsonReqDoc.ParseInsitu<0>(sprCopy.Ptr).HasParseError()) {
		int nMode = GetJsonInt(jsonReqDoc, "Mode", F_AT);
		bool bConference = GetJsonBool(jsonReqDoc, "Conference", F_AT);
		const char*strChanId = GetJsonStr(jsonReqDoc, "ChanId", F_AT);
		const char*strUserData = GetJsonStr(jsonReqDoc, "UserData", F_AT);
		const char*strSipData = GetJsonStr(jsonReqDoc, "SipData", F_AT);
		if (!JsStrNotNull(strChanId)) {// 频道ID不可为空
			remoteInvitation->setResponse("error");
			rtm_call_mgr_->refuseRemoteInvitation(remoteInvitation);
			return;
		}

		bool newCall = false;
		const std::string&strSipAccount = AllocSipAccount();
		if (strSipAccount.length() > 0)
		{
			XAutoLock l(cs_rtc_call_to_sip_);
			if (map_rtc_call_to_sip_.find(remoteInvitation->getCallerId()) == map_rtc_call_to_sip_.end()) {
				newCall = true;
				RtcCallToSip* rtcCallToSip = new RtcCallToSip(*this);
				rtcCallToSip->SetCallerId(remoteInvitation->getCallerId());
				InitRtcCallToSip(remoteInvitation->getCallerId(), strChanId, strSipAccount, strSipData);

				map_rtc_call_to_sip_[remoteInvitation->getCallerId()] = rtcCallToSip;
			}
		}


		if (newCall) {
			rapidjson::Document		jsonDoc;
			rapidjson::StringBuffer jsonStr;
			rapidjson::Writer<rapidjson::StringBuffer> jsonWriter(jsonStr);
			jsonDoc.SetObject();
			jsonDoc.AddMember("Mode", 1, jsonDoc.GetAllocator());
			jsonDoc.AddMember("Conference", bConference, jsonDoc.GetAllocator());
			jsonDoc.AddMember("SipData", "", jsonDoc.GetAllocator());
			jsonDoc.Accept(jsonWriter);
			remoteInvitation->setResponse(jsonStr.GetString());

			rtm_call_mgr_->acceptRemoteInvitation(remoteInvitation);
			long long reqId = 0;
			const char* peerIds[1];
			peerIds[0] = remoteInvitation->getCallerId();
			rtm_service_->subscribePeersOnlineStatus(peerIds, 1, reqId);
		}
		else {
			rtm_call_mgr_->refuseRemoteInvitation(remoteInvitation);
			if (strSipAccount.length() > 0) {
				FreeSipAccount(strSipAccount);
			}
		}
	}
	else {// 呼叫的Content中Json解析失败
		remoteInvitation->setResponse("unkown");
		rtm_call_mgr_->refuseRemoteInvitation(remoteInvitation);
	}
}
void SipRtcMgr::onRemoteInvitationFailure(ARM::IRemoteCallInvitation *remoteInvitation, ARM::REMOTE_INVITATION_ERR_CODE errorCode)
{
}
void SipRtcMgr::onRemoteInvitationCanceled(ARM::IRemoteCallInvitation *remoteInvitation)
{
	EndRtcCallToSip(remoteInvitation->getCallerId());
}

void SipRtcMgr::OnRtcCallToSipClosed(const std::string&strCallerId, int nCode)
{
	MgrEvent* mgrEvent = new MgrEvent();
	mgrEvent->eType = ET_CloseRtcAllToSip;
	mgrEvent->mapStr["CallerId"] = strCallerId;
	mgrEvent->mapInt["Code"] = nCode;

	XAutoLock l(cs_mgr_event_);
	lst_mgr_event_.push_back(mgrEvent);
}

//* Internal function
const std::string&SipRtcMgr::AllocSipAccount()
{
	XAutoLock l(cs_sip_account_);
	MapSipAccount::iterator itsr = map_sip_account_.begin();
	while (itsr != map_sip_account_.end()) {
		if (itsr->second == 0) {
			itsr->second = 1;	// 使用标识
			return itsr->first;
		}
		itsr++;
	}
	return NullStr;
}
void SipRtcMgr::FreeSipAccount(const std::string&strSipAccount)
{
	XAutoLock l(cs_sip_account_);
	MapSipAccount::iterator itsr = map_sip_account_.find(strSipAccount);
	if (itsr != map_sip_account_.end()) {
		itsr->second = 0;	// 标识空闲
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
	mgrEvent->eType = ET_FreeSipAllToRtc;
	mgrEvent->ptr = rtcCallToSip;
	mgrEvent->mapInt["CallId"] = callId;

	XAutoLock l(cs_mgr_event_);
	lst_mgr_event_.push_back(mgrEvent);
}
void SipRtcMgr::EndRtcCallToSip(const std::string&strCallerId)
{
	bool bFindPeer = false;
	std::string strSipAccount;
	{
		XAutoLock l(cs_rtc_call_to_sip_);
		if (map_rtc_call_to_sip_.find(strCallerId) != map_rtc_call_to_sip_.end()) {
			bFindPeer = true;
			RtcCallToSip* rtcCallToSip = map_rtc_call_to_sip_[strCallerId];
			strSipAccount = rtcCallToSip->SipAccount();
			ReleaseRtcCallToSip(rtcCallToSip);

			map_rtc_call_to_sip_.erase(strCallerId);
		}
	}
	if (bFindPeer) {
		long long reqId = 0;
		const char* peerIds[1];
		peerIds[0] = strCallerId.c_str();
		rtm_service_->unsubscribePeersOnlineStatus(peerIds, 1, reqId);
	}
	if (strSipAccount.length() > 0) {
		FreeSipAccount(strSipAccount);
	}
}
void SipRtcMgr::InitRtcCallToSip(const std::string&strCallerId, const std::string&strChanId, const std::string&strSipAccount, const std::string&strSipData)
{
	MgrEvent* mgrEvent = new MgrEvent();
	mgrEvent->eType = ET_InitRtcAllToSip;
	mgrEvent->mapStr["CallerId"] = strCallerId;
	mgrEvent->mapStr["ChanId"] = strChanId;
	mgrEvent->mapStr["SipAccount"] = strSipAccount;
	mgrEvent->mapStr["SipData"] = strSipData;

	XAutoLock l(cs_mgr_event_);
	lst_mgr_event_.push_back(mgrEvent);
}
void SipRtcMgr::ReleaseRtcCallToSip(RtcCallToSip* rtcCallToSip)
{// 线程同步
	MgrEvent* mgrEvent = new MgrEvent();
	mgrEvent->eType = ET_FreeRtcAllToSip;
	mgrEvent->ptr = rtcCallToSip;

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
		if (mgrEvent->eType == ET_InitRtcAllToSip) {
			const std::string&strCallerId = mgrEvent->mapStr["CallerId"];
			const std::string&strChanId = mgrEvent->mapStr["ChanId"];
			const std::string&strSipAccount = mgrEvent->mapStr["SipAccount"];
			const std::string&strSipData = mgrEvent->mapStr["SipData"];
			XAutoLock l(cs_rtc_call_to_sip_);
			if (map_rtc_call_to_sip_.find(strCallerId) != map_rtc_call_to_sip_.end()) {
				RtcCallToSip* rtcCallToSip = map_rtc_call_to_sip_[strCallerId];
				rtcCallToSip->InitSipAccount(str_sip_svr_ip_, n_sip_svr_port_, strSipAccount, str_sip_password_);
				rtcCallToSip->StartTask(str_rtc_rtm_app_id_.c_str(), strChanId, str_ivr_sip_account_, strSipData.c_str());
			}
			else {
				// 呼叫已销毁，需要释放Sip账号
				FreeSipAccount(strSipAccount);		
			}
		}
		else if (mgrEvent->eType == ET_CloseRtcAllToSip) {
			const std::string&strCallerId = mgrEvent->mapStr["CallerId"];
			{
				XAutoLock l(cs_rtc_call_to_sip_);
				if (map_rtc_call_to_sip_.find(strCallerId) != map_rtc_call_to_sip_.end()) {
					rapidjson::Document		jsonDoc;
					rapidjson::StringBuffer jsonStr;
					rapidjson::Writer<rapidjson::StringBuffer> jsonWriter(jsonStr);
					jsonDoc.SetObject();
					jsonDoc.AddMember("Cmd", "EndCall", jsonDoc.GetAllocator());
					jsonDoc.Accept(jsonWriter);
					
					ARM::IMessage *peerMsg = rtm_service_->createMessage(jsonStr.GetString());
					rtm_service_->sendMessageToPeer(strCallerId.c_str(), peerMsg);

					peerMsg->release();
				}
			}
			EndRtcCallToSip(strCallerId);
		}
		else if (mgrEvent->eType == ET_FreeRtcAllToSip) {
			RtcCallToSip*rtcCallToSip = (RtcCallToSip*)mgrEvent->ptr;
			rtcCallToSip->StopTask();
			delete rtcCallToSip;
			rtcCallToSip = NULL;
		}
		else if (mgrEvent->eType == ET_CloseSipAllToRtc) {
			int nCallId = mgrEvent->mapInt["CallId"];
			std::string strCalleeId;
			{
				XAutoLock l(cs_sip_call_to_rtc_);
				if (map_sip_call_to_rtc_.find(nCallId) != map_sip_call_to_rtc_.end()) {
					SipCallToRtc* sipCallToRtc = map_sip_call_to_rtc_[nCallId];
					strCalleeId = sipCallToRtc->CalleeId();

					rapidjson::Document		jsonDoc;
					rapidjson::StringBuffer jsonStr;
					rapidjson::Writer<rapidjson::StringBuffer> jsonWriter(jsonStr);
					jsonDoc.SetObject();
					jsonDoc.AddMember("Cmd", "EndCall", jsonDoc.GetAllocator());
					jsonDoc.Accept(jsonWriter);

					ARM::IMessage *peerMsg = rtm_service_->createMessage(jsonStr.GetString());
					rtm_service_->sendMessageToPeer(strCalleeId.c_str(), peerMsg);

					peerMsg->release();
				}
			}
			if (strCalleeId.length() > 0) {
				EndSipCallToRtc(strCalleeId);
			}
			
		}
		else if (mgrEvent->eType == ET_FreeSipAllToRtc) {
			SipCallToRtc*sipCallToRtc = (SipCallToRtc*)mgrEvent->ptr;
			sipCallToRtc->StopTask();
			delete sipCallToRtc;
			sipCallToRtc = NULL;

			int nCallId = mgrEvent->mapInt["CallId"];
			sip_proxy_->RtcEndCall(nCallId);
		}

		delete mgrEvent;
		mgrEvent = NULL;
	}
}