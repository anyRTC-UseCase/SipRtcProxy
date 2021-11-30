#ifndef __SIP_RTM_TO_H__
#define __SIP_RTM_TO_H__
#include <list>
#include <map>
#include <string>
#include "IArRtmCallManager.h"
#include "IArRtmService.h"
#include "XAutoLock.h"


class SipToRtm;
class SipToRtmEvent
{
public:
	SipToRtmEvent(void) {};
	virtual ~SipToRtmEvent(void) {};

	virtual void OnSipToRtmAcceptCall(int nCallId, const std::string&strCalleeId, SipToRtm*sipToRtm) = 0;
	virtual void OnSipToRtmEndCall(int nCallId) = 0;
};

class SipToRtm : public ARM::IRtmServiceEventHandler, public ARM::IRtmCallEventHandler
{
public:
	SipToRtm(SipToRtmEvent&callback);
	virtual ~SipToRtm(void);

	void SetSipCallInfo(int nCallId, const std::string&strChanId, const std::string&strDisplayName, const std::string&strCalleeId, const std::string&strSipData);
	void StartRtm(const std::string&strRtmAccount);
	void StopRtm();

	void SipEndCall();

	//* For  ARM::IRtmServiceEventHandler
	virtual void onLoginSuccess();
	virtual void onLoginFailure(ARM::LOGIN_ERR_CODE errorCode);
	virtual void onLogout(ARM::LOGOUT_ERR_CODE errorCode);
	virtual void onConnectionStateChanged(ARM::CONNECTION_STATE state, ARM::CONNECTION_CHANGE_REASON reason);
	virtual void onMessageReceivedFromPeer(const char *peerId, const ARM::IMessage *message);

	//* For ARM::IRtmCallEventHandler
	virtual void onLocalInvitationReceivedByPeer(ARM::ILocalCallInvitation *localInvitation);
	virtual void onLocalInvitationCanceled(ARM::ILocalCallInvitation *localInvitation);
	virtual void onLocalInvitationFailure(ARM::ILocalCallInvitation *localInvitation, ARM::LOCAL_INVITATION_ERR_CODE errorCode);
	virtual void onLocalInvitationAccepted(ARM::ILocalCallInvitation *localInvitation, const char *response);
	virtual void onLocalInvitationRefused(ARM::ILocalCallInvitation *localInvitation, const char *response);

private:
	SipToRtmEvent		&callback_;
	ARM::IRtmService	*rtm_service_;
	ARM::IRtmCallManager*rtm_call_mgr_;

	bool	rtm_logined_;
	bool	rtm_lost_connection_;
	bool	rtm_has_call_;

	int		n_call_id_;
	std::string str_display_name_;
	std::string str_callee_id_;
	std::string str_chan_id_;
	std::string str_sip_data_;

	std::string			str_rtm_account_;

	XCritSec			cs_local_session_;
	std::string			str_local_session_id_;
};


#endif	// __SIP_RTM_TO_H__
