#ifndef __RTM_TO_SIP_H__
#define __RTM_TO_SIP_H__
#include <list>
#include <map>
#include <string>
#include "IArRtmCallManager.h"
#include "IArRtmService.h"
#include "XAutoLock.h"

class RtmToSip;
class RtmToSipEvent
{
public:
	RtmToSipEvent(void) {};
	virtual ~RtmToSipEvent(void) {};

	virtual void OnRtmToSipMakeCall(const std::string&strSessionId, const std::string&strCallerId, const std::string&strChanId, const std::string&strCallData, const std::string&strCallNumber, bool bPstn, RtmToSip*rtmToSip) = 0;
	virtual void OnRtmToSipEndCall(int nCallId) = 0;
};

class RtmToSip : public ARM::IRtmServiceEventHandler, public ARM::IRtmCallEventHandler
{
public:
	RtmToSip(RtmToSipEvent&callback);
	virtual ~RtmToSip(void);

	void StartRtm(const std::string&strRtmAccount);
	void StopRtm();

	void SipEndCall();
	void SetSipCallId(const std::string&strSessionId, int nCallId);
	const std::string&RtmAccount() { return str_rtm_account_; };

	bool ProcessMsg();

	//* For  ARM::IRtmServiceEventHandler
	virtual void onLoginSuccess();
	virtual void onLoginFailure(ARM::LOGIN_ERR_CODE errorCode);
	virtual void onLogout(ARM::LOGOUT_ERR_CODE errorCode);
	virtual void onConnectionStateChanged(ARM::CONNECTION_STATE state, ARM::CONNECTION_CHANGE_REASON reason);
	virtual void onMessageReceivedFromPeer(const char *peerId, const ARM::IMessage *message);

	//* For ARM::IRtmCallEventHandler
	virtual void onRemoteInvitationRefused(ARM::IRemoteCallInvitation *remoteInvitation);
	virtual void onRemoteInvitationAccepted(ARM::IRemoteCallInvitation *remoteInvitation);
	virtual void onRemoteInvitationReceived(ARM::IRemoteCallInvitation *remoteInvitation);
	virtual void onRemoteInvitationFailure(ARM::IRemoteCallInvitation *remoteInvitation, ARM::REMOTE_INVITATION_ERR_CODE errorCode);
	virtual void onRemoteInvitationCanceled(ARM::IRemoteCallInvitation *remoteInvitation);


private:
	RtmToSipEvent		&callback_;
	ARM::IRtmService	*rtm_service_;
	ARM::IRtmCallManager*rtm_call_mgr_;

	bool	rtm_logined_;
	bool	rtm_lost_connection_;
	bool	rtm_has_call_;

	int		n_call_id_;
	std::string str_caller_id_;
	std::string str_chan_id_;

	std::string			str_rtm_account_;

	XCritSec			cs_local_session_;
	std::string			str_local_session_id_;
};

#endif	// __RTM_TO_SIP_H__