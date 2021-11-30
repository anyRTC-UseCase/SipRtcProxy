#include "SipToRtm.h"
#include "RtcLog.h"
#include <assert.h>
#include "SipRtcMgr.h"
#include "rapidjson/json_value.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"	
#include "rapidjson/stringbuffer.h"

SipToRtm::SipToRtm(SipToRtmEvent&callback)
	: callback_(callback)
	, rtm_service_(NULL)
	, rtm_call_mgr_(NULL)
	, rtm_logined_(false)
	, rtm_lost_connection_(false)
	, rtm_has_call_(false)
	, n_call_id_(-1)
{

}
SipToRtm::~SipToRtm(void)
{
	assert(rtm_service_ == NULL);
}

void SipToRtm::SetSipCallInfo(int nCallId, const std::string&strChanId, const std::string&strDisplayName, const std::string&strCalleeId, const std::string&strSipData)
{
	n_call_id_ = nCallId;
	str_chan_id_ = strChanId;
	str_display_name_ = strDisplayName;
	str_callee_id_ = strCalleeId;
	str_sip_data_ = strSipData;
}

void SipToRtm::StartRtm(const std::string&strRtmAccount)
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
void SipToRtm::StopRtm()
{
	if (rtm_service_ != NULL) {
		rtm_call_mgr_->release();
		rtm_call_mgr_ = NULL;

		rtm_service_->logout();
		rtm_service_->release();
		rtm_service_ = NULL;
	}
}

void SipToRtm::SipEndCall()
{
	XAutoLock l(cs_local_session_);
	if (n_call_id_ != -1) {
		rapidjson::Document		jsonDoc;
		rapidjson::StringBuffer jsonStr;
		rapidjson::Writer<rapidjson::StringBuffer> jsonWriter(jsonStr);
		jsonDoc.SetObject();
		jsonDoc.AddMember("Cmd", "EndCall", jsonDoc.GetAllocator());
		jsonDoc.Accept(jsonWriter);

		ARM::IMessage *peerMsg = rtm_service_->createMessage(jsonStr.GetString());
		rtm_service_->sendMessageToPeer(str_callee_id_.c_str(), peerMsg);

		peerMsg->release();

		n_call_id_ = -1;
		rtm_has_call_ = false;
		str_callee_id_.clear();
	}
}

//* For  ARM::IRtmServiceEventHandler
void SipToRtm::onLoginSuccess()
{
	rtm_logined_ = true;

	if (!rtm_has_call_) {
		rtm_has_call_ = true;

		ARM::ILocalCallInvitation* localInv = rtm_call_mgr_->createLocalCallInvitation(str_callee_id_.c_str());
		rapidjson::Document		jsonDoc;
		rapidjson::StringBuffer jsonStr;
		rapidjson::Writer<rapidjson::StringBuffer> jsonWriter(jsonStr);
		jsonDoc.SetObject();
		jsonDoc.AddMember("Mode", 1, jsonDoc.GetAllocator());
		jsonDoc.AddMember("Conference", 0, jsonDoc.GetAllocator());
		jsonDoc.AddMember("ChanId", str_chan_id_.c_str(), jsonDoc.GetAllocator());
		if (str_display_name_.length() > 0) {
			jsonDoc.AddMember("DisplayName", str_display_name_.c_str(), jsonDoc.GetAllocator());
		}
		jsonDoc.AddMember("SipData", str_sip_data_.c_str(), jsonDoc.GetAllocator());
		jsonDoc.Accept(jsonWriter);
		localInv->setContent(jsonStr.GetString());
		rtm_call_mgr_->sendLocalInvitation(localInv);
	}
}
void SipToRtm::onLoginFailure(ARM::LOGIN_ERR_CODE errorCode)
{
	rtm_logined_ = false;
	rtm_lost_connection_ = true;
}
void SipToRtm::onLogout(ARM::LOGOUT_ERR_CODE errorCode)
{
	rtm_logined_ = false;
	rtm_lost_connection_ = true;
}
void SipToRtm::onConnectionStateChanged(ARM::CONNECTION_STATE state, ARM::CONNECTION_CHANGE_REASON reason)
{
	if (state == ARM::CONNECTION_STATE_DISCONNECTED) {
		rtm_logined_ = false;
		rtm_lost_connection_ = true;
	}
}
void SipToRtm::onMessageReceivedFromPeer(const char *peerId, const ARM::IMessage *message)
{
	rapidjson::Document		jsonReqDoc;
	JsonStr sprCopy(message->getText(), strlen(message->getText()));
	if (!jsonReqDoc.ParseInsitu<0>(sprCopy.Ptr).HasParseError()) {
		const char*strCmd = GetJsonStr(jsonReqDoc, "Cmd", F_AT);
		if (strcmp("EndCall", strCmd) == 0) {
			XAutoLock l(cs_local_session_);
			if (n_call_id_ != -1 && str_callee_id_.compare(peerId) == 0) {
				callback_.OnSipToRtmEndCall(n_call_id_);
				n_call_id_ = -1;
				rtm_has_call_ = false;
				str_callee_id_.clear();
			}
		}
	}
}

//* For ARM::IRtmCallEventHandler
void SipToRtm::onLocalInvitationReceivedByPeer(ARM::ILocalCallInvitation *localInvitation)
{

}
void SipToRtm::onLocalInvitationCanceled(ARM::ILocalCallInvitation *localInvitation)
{
	XAutoLock l(cs_local_session_);
	if (n_call_id_ != -1 && str_callee_id_.compare(localInvitation->getCalleeId()) == 0) {
		callback_.OnSipToRtmEndCall(n_call_id_);
		n_call_id_ = -1;
		rtm_has_call_ = false;
		str_callee_id_.clear();
	}
}
void SipToRtm::onLocalInvitationFailure(ARM::ILocalCallInvitation *localInvitation, ARM::LOCAL_INVITATION_ERR_CODE errorCode)
{
	XAutoLock l(cs_local_session_);
	if (n_call_id_ != -1 && str_callee_id_.compare(localInvitation->getCalleeId()) == 0) {
		callback_.OnSipToRtmEndCall(n_call_id_);
		n_call_id_ = -1;
		rtm_has_call_ = false;
		str_callee_id_.clear();
	}
}
void SipToRtm::onLocalInvitationAccepted(ARM::ILocalCallInvitation *localInvitation, const char *response)
{
	callback_.OnSipToRtmAcceptCall(n_call_id_, str_callee_id_, this);
}
void SipToRtm::onLocalInvitationRefused(ARM::ILocalCallInvitation *localInvitation, const char *response)
{
	XAutoLock l(cs_local_session_);
	if (n_call_id_ != -1 && str_callee_id_.compare(localInvitation->getCalleeId()) == 0) {
		callback_.OnSipToRtmEndCall(n_call_id_);
		n_call_id_ = -1;
		rtm_has_call_ = false;
		str_callee_id_.clear();
	}
}
