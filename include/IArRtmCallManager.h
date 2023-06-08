//
//  AR's RTM SDK
//
//
//  Copyright (c) 2019 AR.io. All rights reserved.
//
#pragma once
#include "IArRtmService.h"

namespace ar {
  namespace rtm {
      
    /**
     @brief <b>RETURNED TO THE CALLER.</b> States of an outgoing call invitation.
     */
    enum LOCAL_INVITATION_STATE {
        
      /**
       0: <b>RETURNED TO THE CALLER.</b> The initial state of a call invitation (idle).
       */
      LOCAL_INVITATION_STATE_IDLE = 0,
        
      /**
       1: <b>RETURNED TO THE CALLER.</b> The call invitation is sent to the callee.
       */
      LOCAL_INVITATION_STATE_SENT_TO_REMOTE = 1,
        
      /**
       2: <b>RETURNED TO THE CALLER.</b> The call invitation is received by the callee.
       */
      LOCAL_INVITATION_STATE_RECEIVED_BY_REMOTE = 2,
        
      /**
       3: <b>RETURNED TO THE CALLER.</b> The call invitation is accepted by the callee.
       */
      LOCAL_INVITATION_STATE_ACCEPTED_BY_REMOTE = 3,
        
      /**
       4: <b>RETURNED TO THE CALLER.</b> The call invitation is declined by the callee.
       */
      LOCAL_INVITATION_STATE_REFUSED_BY_REMOTE = 4,
        
      /**
       5: <b>RETURNED TO THE CALLER.</b> You have canceled the call invitation.
       */
      LOCAL_INVITATION_STATE_CANCELED = 5,
        
      /**
       6: <b>RETURNED TO THE CALLER.</b> The call invitation fails.
       */
      LOCAL_INVITATION_STATE_FAILURE = 6,
    };

    /**
     @brief <b>RETURNED TO THE CALLEE.</b> States of an incoming call invitation.
     */
    enum REMOTE_INVITATION_STATE {
        
      /**
       0: <b>RETURNED TO THE CALLEE.</b> The initial state of a call invitation (idle).
       */
      REMOTE_INVITATION_STATE_IDLE = 0,
        
      /**
       1: <b>RETURNED TO THE CALLEE.</b> A call invitation from a remote caller is received.
       */
      REMOTE_INVITATION_STATE_INVITATION_RECEIVED = 1,
        
      /**
       2: <b>RETURNED TO THE CALLEE.</b> The message is sent to the caller that the call invitation is accepted.
       */
      REMOTE_INVITATION_STATE_ACCEPT_SENT_TO_LOCAL = 2,
        
      /**
       3: <b>RETURNED TO THE CALLEE.</b> You have declined the call invitation.
       */
      REMOTE_INVITATION_STATE_REFUSED = 3,
        
      /**
       4: <b>RETURNED TO THE CALLEE.</b> You have accepted the call invitation.
       */
      REMOTE_INVITATION_STATE_ACCEPTED = 4,
        
      /**
       5: <b>RETURNED TO THE CALLEE.</b> The call invitation is canceled by the remote caller.
       */
      REMOTE_INVITATION_STATE_CANCELED = 5,
        
      /**
       6: <b>RETURNED TO THE CALLEE.</b> The call invitation fails.
       */
      REMOTE_INVITATION_STATE_FAILURE = 6,
    };

    /**
     @brief <b>RETURNED TO THE CALLER.</b> Error codes of an outgoing call invitation.
     */
    enum LOCAL_INVITATION_ERR_CODE {
        
      /**
       0: <b>RETURNED TO THE CALLER.</b> The outgoing invitation succeeds.
       */
      LOCAL_INVITATION_ERR_OK = 0,
        
      /**
       1: <b>RETURNED TO THE CALLER.</b> The callee is offline.
        
       The SDK performs the following:
       - Keeps resending the call invitation to the callee, if the callee is offline.
       - Returns this error code, if the callee is still offline 30 seconds since the call invitation is sent.
       */
      LOCAL_INVITATION_ERR_PEER_OFFLINE = 1,
        
      /**
       2: <b>RETURNED TO THE CALLER.</b> The callee is online but has not ACKed to the call invitation 30 seconds since it is sent.
       */
      LOCAL_INVITATION_ERR_PEER_NO_RESPONSE = 2,
        
      /**
       3: <b>RETURNED TO THE CALLER. SAVED FOR FUTURE USE.</b> The call invitation expires 60 seconds since it is sent, if the callee ACKs to the call invitation but neither the caller or callee takes any further action (cancel, accpet, or decline it).
       */
      LOCAL_INVITATION_ERR_INVITATION_EXPIRE = 3,
        
      /**
       4: <b>RETURNED TO THE CALLER.</b> The caller is not logged in.
       */
      LOCAL_INVITATION_ERR_NOT_LOGGEDIN = 4,
    };
      
    /**
     @brief <b>RETURNED TO THE CALLEE.</b> Error codes of an incoming call invitation.
     */
    enum REMOTE_INVITATION_ERR_CODE {
        
      /**
       0: <b>RETURNED TO THE CALLEE.</b> The incoming calll invitation succeeds.
       */
      REMOTE_INVITATION_ERR_OK = 0,
        
      /**
       1: <b>RETURNED TO THE CALLEE.</b> The call invitation received by the callee fails: the callee is not online.
       */
      REMOTE_INVITATION_ERR_PEER_OFFLINE = 1,
        
      /**
       2: <b>RETURNED TO THE CALLEE.</b> The call invitation received by callee fails: The callee does not ACK within a set time after the callee accepts the call invitation. This is usually a result of a network interruption.
       */
      REMOTE_INVITATION_ERR_ACCEPT_FAILURE = 2,
        
      /**
       3: <b>RETURNED TO THE CALLEE.</b> The call invitation expires 60 seconds since it is sent, if the callee ACKs to the call invitation but neither the caller or callee takes any further action (cancel, accpet, or decline it).
       */
      REMOTE_INVITATION_ERR_INVITATION_EXPIRE = 3,
    };

    /**
     @brief Error codes of the call invitation methods.
     */
    enum INVITATION_API_CALL_ERR_CODE {
        
      /**
       0: The method call succeeds.
       */
      INVITATION_API_CALL_ERR_OK = 0,
        
      /**
       1: The method call fails. The argument is invalid, for example, the value of the @p content parameter exceeds 8K bytes.
       */
      INVITATION_API_CALL_ERR_INVALID_ARGUMENT = 1,
        
      /**
       2: The method call fails. The call invitation has not started.
       */
      INVITATION_API_CALL_ERR_NOT_STARTED = 2,
        
      /**
       3: The method call fails. The call invitation has ended.
       */
      INVITATION_API_CALL_ERR_ALREADY_END = 3, // accepted, failure, canceled, refused
        
      /**
       4: The method call fails. The call invitation is already accepted.
       */
      INVITATION_API_CALL_ERR_ALREADY_ACCEPT = 4,   // more details
        
      /**
       5: The method call fails. The call invitation is already sent.
       */
      INVITATION_API_CALL_ERR_ALREADY_SENT = 5,
    };

    /**
     The class allowing the caller to retrieve the attributes of an outgoing call invitation.
     */
    class ILocalCallInvitation
    {
    protected:
      virtual ~ILocalCallInvitation() {}
    public:
        
      /**
       Allows the caller to get the User ID of the callee.
       */
      virtual const char *getCalleeId() const = 0;
        
      /**
       Allows the caller to set the call invitation content.
    
       @param content The content of the call invitation. The @p content must not exceed 8 KB in length if encoded in UTF-8.
       */
      virtual void setContent(const char *content) = 0;
        
      /**
       Allows the caller to get the call invitation content.
       
       @note The caller sets the call invitation content using the \ref ar::rtm::ILocalCallInvitation::setContent "setContent" method.
       */
      virtual const char *getContent() const = 0;
        
      /**
       Sets the channel ID.
       
       @note To intercommunicate with the legacy AR Signaling SDK, you MUST set the channel ID. However, even if the callee successfully accepts the call invitation, the Agora RTM SDK does not join the channel of the specified channel ID. 
       
       @param channelId The channel ID to be set.
       */
      virtual void setChannelId(const char *channelId) = 0;
        
      /**
       Gets the channel ID.
       */
      virtual const char *getChannelId() const = 0;
        
      /**
       Allows the caller to get the callee's response to the call invitation.
       
       @note The callee sets his/her response using the \ref ar::rtm::IRemoteCallInvitation::setResponse "setResponse" method.
       */
      virtual const char *getResponse() const = 0;
        
      /**
       Allows the caller to get the state of the outgoing call invitation.

       @return State of the outgoing call invitation. See: #LOCAL_INVITATION_STATE.
       */
      virtual LOCAL_INVITATION_STATE getState() const = 0;
        
      /**
       Releases all resources used by the ILocalCallInvitation instance.
       */
      virtual void release() = 0;
    };

    /**
     The class allowing the callee to retrieve the attributes of an incoming call invitation.
     */
    class IRemoteCallInvitation
    {
    protected:
      virtual ~IRemoteCallInvitation() {}
    public:
        
      /**
       Allows the callee to get the User ID of the caller.
       */
      virtual const char *getCallerId() const = 0;
      
      /**
       Allows the callee to get the call invitation content set by the caller.
       
       @note The caller sets the call invitation content using the \ref ar::rtm::ILocalCallInvitation::setContent "setContent" method.
       */
      virtual const char *getContent() const = 0;
        
      /**
       Allows the callee to set a response to the call invitation.

       @param response The callee's response to the call invitation. The @p response must not exceed 8 KB in length if encoded in UTF-8.
       */
      virtual void setResponse(const char *response) = 0;
      
      /**
       Allows the callee to get his/her response to the incoming call invitation.
       
       @note The callee sets a response to the incoming call invitation using the \ref ar::rtm::IRemoteCallInvitation::setResponse "setResponse" method.
       */
      virtual const char *getResponse() const = 0;
        
      /**
       Gets the channel ID.
       */
      virtual const char *getChannelId() const = 0;
      
      /**
       Allows the callee to get the state of the incoming call invitation.
       
       @return The state of the incoming call invitation See: #REMOTE_INVITATION_STATE.
       */
      virtual REMOTE_INVITATION_STATE getState() const = 0;
        
      /**
       Releases all resources used by the IRemoteCallInvitation instance.
       */
      virtual void release() = 0;
    };

    /**
     Callbacks for the call invitation methods.
     */
    class IRtmCallEventHandler
    {
    public:
      virtual ~IRtmCallEventHandler()
      {
      }
        
      /**
       Callback to the caller: occurs when the callee receives the call invitation.
	
	   @param localInvitation An ILocalCallInvitation object.
	   */
      virtual void onLocalInvitationReceivedByPeer(ILocalCallInvitation *localInvitation)
      {
        (ILocalCallInvitation *) localInvitation;
      }
        
      /**
       Callback to the caller: occurs when the caller cancels a call invitation.
	 
	   @param localInvitation An ILocalCallInvitation object.
       */
      virtual void onLocalInvitationCanceled(ILocalCallInvitation *localInvitation)
      {
        (ILocalCallInvitation *) localInvitation;
      }
        
      /**
       Callback to the caller: occurs when the life cycle of the outgoing call invitation ends in failure.
	 
       @param localInvitation An ILocalCallInvitation object.
       @param errorCode The error code. See #LOCAL_INVITATION_ERR_CODE.
       */
      virtual void onLocalInvitationFailure(ILocalCallInvitation *localInvitation, LOCAL_INVITATION_ERR_CODE errorCode)
      {
        (ILocalCallInvitation *) localInvitation;
        (LOCAL_INVITATION_ERR_CODE) errorCode;
      }
    
      /**
       Callback to the caller: occurs when the callee accepts the call invitation.
	 
	   @param localInvitation An ILocalCallInvitation object.
       @param response The callee's response to the call invitation.
	   */
      virtual void onLocalInvitationAccepted(ILocalCallInvitation *localInvitation, const char *response)
      {
        (ILocalCallInvitation *) localInvitation;
        (const char *) response;
      }
      
      /**
       Callback to the caller: occurs when the callee refuses the call invitation.

       @param localInvitation An ILocalCallInvitation object.
       @param response The callee's response to the call invitation.
       */
      virtual void onLocalInvitationRefused(ILocalCallInvitation *localInvitation, const char *response)
      {
        (ILocalCallInvitation *) localInvitation;
        (const char *) response;
      }
        
      /**
       Callback for the callee: occurs when the callee refuses a call invitation.
    
       @param remoteInvitation An IRemoteCallInvitation object.
       */
      virtual void onRemoteInvitationRefused(IRemoteCallInvitation *remoteInvitation)
      {
        (IRemoteCallInvitation *) remoteInvitation;
      }
        
      /**
       Callback to the callee: occurs when the callee accepts a call invitation.
    
       @param remoteInvitation An IRemoteCallInvitation object.
       */
      virtual void onRemoteInvitationAccepted(IRemoteCallInvitation *remoteInvitation)
      {
        (IRemoteCallInvitation *) remoteInvitation;
      }
        
      /**
       Callback to the callee: occurs when the callee receives a call invitation.
	
	   @param remoteInvitation An IRemoteCallInvitation object.
	   */
      virtual void onRemoteInvitationReceived(IRemoteCallInvitation *remoteInvitation)
      {
        (IRemoteCallInvitation *) remoteInvitation;
      }
        
      /**
       Callback to the callee: occurs when the life cycle of the incoming call invitation ends in failure.
	
       @param remoteInvitation An IRemoteCallInvitation object.
       @param errorCode The error code. See #REMOTE_INVITATION_ERR_CODE.
       */
      virtual void onRemoteInvitationFailure(IRemoteCallInvitation *remoteInvitation, REMOTE_INVITATION_ERR_CODE errorCode)
      {
        (IRemoteCallInvitation *) remoteInvitation;
      }
        
      /**
       Callback to the callee: occurs when the caller cancels the call invitation.
	
	   @param remoteInvitation An IRemoteCallInvitation object.
	   */
      virtual void onRemoteInvitationCanceled(IRemoteCallInvitation *remoteInvitation)
      {
        (IRemoteCallInvitation *) remoteInvitation;
      }
    };
      
    /**
     RTM call manager methods.
     */
    class IRtmCallManager
    {
    protected:
      virtual ~IRtmCallManager() {}
    public:
        
      /**
       Allows the caller to send a call invitation to the callee.
    
       @param invitation An \ref ar::rtm::ILocalCallInvitation "ILocalCallInvitation" object.
       @return
       - 0: Success.
       - <0: Failure. See #INVITATION_API_CALL_ERR_CODE for the error code.
       */
      virtual int sendLocalInvitation(ILocalCallInvitation *invitation)= 0;
        
      /**
       Allows the callee to accept an incoming call invitation.
     
       @param invitation An \ref ar::rtm::IRemoteCallInvitation "IRemoteCallInvitation" object.
       @return
       - 0: Success.
       - <0: Failure. See #INVITATION_API_CALL_ERR_CODE for the error code.
       */
      virtual int acceptRemoteInvitation(IRemoteCallInvitation *invitation) = 0;
        
      /**
       Allows the callee to decline an incoming call invitation.
    
       @param invitation An \ref ar::rtm::IRemoteCallInvitation "IRemoteCallInvitation" object.
       @return
       - 0: Success.
       - <0: Failure. See #INVITATION_API_CALL_ERR_CODE for the error code.
       */
      virtual int refuseRemoteInvitation(IRemoteCallInvitation *invitation) = 0;
        
      /**
       Allows the caller to cancel an outgoing call invitation.

       @param invitation An \ref ar::rtm::ILocalCallInvitation "ILocalCallInvitation" object.
       @return
       - 0: Success.
       - <0: Failure. See #INVITATION_API_CALL_ERR_CODE for the error code.
       */
      virtual int cancelLocalInvitation(ILocalCallInvitation *invitation,const char*reason) = 0;
        
       /**
        Creates an ILocalCallInvitation object.
	
	    @param calleeId The Callee's user ID.
	    @return An \ref ar::rtm::ILocalCallInvitation "ILocalCallInvitation" object.
        */
      virtual ILocalCallInvitation *createLocalCallInvitation(const char *calleeId) = 0;
        
      // sync_call
      /**
       Releases all resources used by the IRtmCallManager instance.
       */
      virtual void release() = 0;
    };
  }
}
