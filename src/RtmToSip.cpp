#include "RtmToSip.h"
#include "RtcLog.h"
#include <assert.h>
#include "SipRtcMgr.h"
#include "rapidjson/json_value.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"	
#include "rapidjson/stringbuffer.h"

RtmToSip::RtmToSip(RtmToSipEvent&callback)
: callback_(callback)
, rtm_service_(NULL)
, rtm_call_mgr_(NULL)
, rtm_logined_(false)
, rtm_lost_connection_(false)
, rtm_has_call_(false)
, n_call_id_(-1)
{

}
RtmToSip::~RtmToSip(void)
{

}

void RtmToSip::StartRtm(const std::string&strRtmAccount)
{
	if (rtm_service_ == NULL) {
		rtm_service_ = ARM::createRtmService();
		rtm_service_->initialize(SipRtcMgr::Inst().RtcRtmAppId().c_str(), this);
		//rtm_service_->setLogFile("./sip_rtm.log");
		if (SipRtcMgr::Inst().RtmSvrAddr().length() > 0 && SipRtcMgr::Inst().RtmSvrPort() > 0) {
			char strParams[1024];
			sprintf(strParams, "{\"Cmd\":\"ConfPriCloudAddr\", \"ServerAdd\": \"%s\", \"Port\": %d}", SipRtcMgr::Inst().RtmSvrAddr().c_str(), SipRtcMgr::Inst().RtmSvrPort());
			rtm_service_->setParameters(strParams);
		}
		str_rtm_account_ = strRtmAccount;
		int ret = rtm_service_->login(NULL, str_rtm_account_.c_str());
		printf("RtmService login: %d\r\n", ret);

		rtm_call_mgr_ = rtm_service_->getRtmCallManager(this);
	}
}
void RtmToSip::StopRtm()
{
	if (rtm_service_ != NULL) {
		rtm_call_mgr_->release();
		rtm_call_mgr_ = NULL;

		rtm_service_->logout();
		rtm_service_->release();
		rtm_service_ = NULL;
	}
}

void RtmToSip::SipEndCall()
{
	std::string strEndCallerId;
	{
		XAutoLock l(cs_local_session_);
		if (n_call_id_ != -1) {
			strEndCallerId = str_caller_id_;

			n_call_id_ = -1;
			rtm_has_call_ = false;
			str_caller_id_.clear();
		}
	}

	if (strEndCallerId.length() > 0) {
		rapidjson::Document		jsonDoc;
		rapidjson::StringBuffer jsonStr;
		rapidjson::Writer<rapidjson::StringBuffer> jsonWriter(jsonStr);
		jsonDoc.SetObject();
		jsonDoc.AddMember("Cmd", "EndCall", jsonDoc.GetAllocator());
		jsonDoc.Accept(jsonWriter);

		ARM::IMessage *peerMsg = rtm_service_->createMessage(jsonStr.GetString());
		rtm_service_->sendMessageToPeer(strEndCallerId.c_str(), peerMsg);

		peerMsg->release();
	}
}

void RtmToSip::SetSipCallId(const std::string&strSessionId, int nCallId)
{
	XAutoLock l(cs_local_session_);
	if (str_local_session_id_.compare(strSessionId) == 0) {
		n_call_id_ = nCallId;
	}
	else {
		callback_.OnRtmToSipEndCall(nCallId);
	}
}

bool RtmToSip::ProcessMsg()
{
	bool bReconnect = false;
	{
		XAutoLock l(cs_local_session_);
		if (rtm_lost_connection_) {
			rtm_lost_connection_ = false;
			if (n_call_id_ != -1) {
				callback_.OnRtmToSipEndCall(n_call_id_);
				n_call_id_ = -1;
				rtm_has_call_ = false;
				str_caller_id_.clear();
			}
			bReconnect = true;
		}
	}

	if (bReconnect) {
		rtm_service_->logout();
		int nRet = rtm_service_->login(NULL, str_rtm_account_.c_str());
		if (nRet != 0) {
			printf("RtmToSip(%s) do login: %d\r\n", str_rtm_account_.c_str(), nRet);
		}
	}
	return true;
}


//* For  ARM::IRtmServiceEventHandler
void RtmToSip::onLoginSuccess()
{
	rtm_logined_ = true;
}
void RtmToSip::onLoginFailure(ARM::LOGIN_ERR_CODE errorCode)
{
	rtm_logined_ = false;
	rtm_lost_connection_ = true;
}
void RtmToSip::onLogout(ARM::LOGOUT_ERR_CODE errorCode)
{
	rtm_logined_ = false;
	rtm_lost_connection_ = true;
}
void RtmToSip::onConnectionStateChanged(ARM::CONNECTION_STATE state, ARM::CONNECTION_CHANGE_REASON reason)
{
	if (state == ARM::CONNECTION_STATE_DISCONNECTED && reason != ARM::CONNECTION_CHANGE_REASON_LOGOUT) {
		rtm_logined_ = false;
		rtm_lost_connection_ = true;
	}
}
void RtmToSip::onMessageReceivedFromPeer(const char *peerId, const ARM::IMessage *message)
{
	rapidjson::Document		jsonReqDoc;
	JsonStr sprCopy(message->getText(), strlen(message->getText()));
	if (!jsonReqDoc.ParseInsitu<0>(sprCopy.Ptr).HasParseError()) {
		const char*strCmd = GetJsonStr(jsonReqDoc, "Cmd", F_AT);
		if (strcmp("EndCall", strCmd) == 0) {
			XAutoLock l(cs_local_session_);
			if (n_call_id_ != -1 && str_caller_id_.compare(peerId) == 0) {
				callback_.OnRtmToSipEndCall(n_call_id_);
				n_call_id_ = -1;
				rtm_has_call_ = false;
				str_caller_id_.clear();
			}
		}
	}
}

//* For ARM::IRtmCallEventHandler
void RtmToSip::onRemoteInvitationRefused(ARM::IRemoteCallInvitation *remoteInvitation)
{

}
void RtmToSip::onRemoteInvitationAccepted(ARM::IRemoteCallInvitation *remoteInvitation)
{

}
void RtmToSip::onRemoteInvitationReceived(ARM::IRemoteCallInvitation *remoteInvitation)
{
	rapidjson::Document		jsonReqDoc;
	JsonStr sprCopy(remoteInvitation->getContent(), strlen(remoteInvitation->getContent()));
	if (!jsonReqDoc.ParseInsitu<0>(sprCopy.Ptr).HasParseError()) {
		int nMode = GetJsonInt(jsonReqDoc, "Mode", F_AT);
		bool bConference = GetJsonBool(jsonReqDoc, "Conference", F_AT);
		std::string strChanId = GetJsonStr(jsonReqDoc, "ChanId", F_AT);
		const char*strUserData = GetJsonStr(jsonReqDoc, "UserData", F_AT);
		const char*strSipData = GetJsonStr(jsonReqDoc, "SipData", F_AT);
		bool bPstn = GetJsonBool(jsonReqDoc, "Pstn", F_AT);
		if (strChanId.length() == 0) {//* 主叫不带ChanId，则需要生成一个
			XGetRandomStr(strChanId, 9);
		}

		XAutoLock l(cs_local_session_);
		if (rtm_has_call_) {
			remoteInvitation->setResponse("busy");
			rtm_call_mgr_->refuseRemoteInvitation(remoteInvitation);
			return;
		}
		rtm_has_call_ = true;
		str_caller_id_ = remoteInvitation->getCallerId();
		str_chan_id_ = strChanId;
		XGetRandomStr(str_local_session_id_, 16);

		rapidjson::Document		jsonDoc;
		rapidjson::StringBuffer jsonStr;
		rapidjson::Writer<rapidjson::StringBuffer> jsonWriter(jsonStr);
		jsonDoc.SetObject();
		jsonDoc.AddMember("Mode", nMode, jsonDoc.GetAllocator());
		jsonDoc.AddMember("Conference", bConference, jsonDoc.GetAllocator());
		jsonDoc.AddMember("ChanId", strChanId.c_str(), jsonDoc.GetAllocator());
		jsonDoc.AddMember("SipNumber", str_rtm_account_.c_str(), jsonDoc.GetAllocator());
		jsonDoc.AddMember("SipData", "", jsonDoc.GetAllocator());
		jsonDoc.Accept(jsonWriter);
		remoteInvitation->setResponse(jsonStr.GetString());

		rtm_call_mgr_->acceptRemoteInvitation(remoteInvitation);

		std::string strCallNumber = str_caller_id_;
		if (strlen(strSipData) > 0) {
			strCallNumber = strSipData;
		}

		callback_.OnRtmToSipMakeCall(str_local_session_id_, str_caller_id_, strChanId, strUserData, strCallNumber, bPstn, this);
	}
	else {// 呼叫的Content中Json解析失败
		remoteInvitation->setResponse("unkown");
		rtm_call_mgr_->refuseRemoteInvitation(remoteInvitation);
	}
}
void RtmToSip::onRemoteInvitationFailure(ARM::IRemoteCallInvitation *remoteInvitation, ARM::REMOTE_INVITATION_ERR_CODE errorCode)
{

}
void RtmToSip::onRemoteInvitationCanceled(ARM::IRemoteCallInvitation *remoteInvitation)
{
	XAutoLock l(cs_local_session_);
	if (n_call_id_ != -1 && str_caller_id_.compare(remoteInvitation->getCallerId()) == 0) {
		callback_.OnRtmToSipEndCall(n_call_id_);
		n_call_id_ = -1;
		rtm_has_call_ = false;
		str_caller_id_.clear();
	}	
}

