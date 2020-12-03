//
//  The AR RTM SDK
//
//
//  Copyright (c) 2020 AR.io. All rights reserved.
//
#pragma once

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdint>
#define ARM_CALL __cdecl
#if defined(ARRTC_EXPORT)
#define ARM_API extern "C" __declspec(dllexport)
#else
#define ARM_API extern "C" __declspec(dllimport)
#endif
#define _ARM_CPP_API

#elif defined(__APPLE__)
#include <cstdint>
#define ARM_API __attribute__((visibility("default"))) extern "C"
#define ARM_CALL
#define _ARM_CPP_API

#elif defined(__ANDROID__) || defined(__linux__)
#if defined(__ANDROID__) && defined(FEATURE_RTM_STANDALONE_SDK)
#define ARM_API extern "C"
#define _ARM_CPP_API
#else
#define ARM_API extern "C" __attribute__((visibility("default")))
#define _ARM_CPP_API __attribute__((visibility("default")))
#endif
#define ARM_CALL

#else
#define ARM_API extern "C"
#define ARM_CALL
#define _ARM_CPP_API
#endif
#include <stdint.h>

/**
 The prefix for ending a call. You can use it with the \ref ar::rtm::IRtmService::sendMessageToPeer(const char *peerId, const IMessage *message, const SendMessageOptions &options) "sendMessageToPeer" method to be compatible with the endCall method of the legacy AR Signaling SDK.
 */
#define AR_RTM_ENDCALL_PREFIX "ArRTMLegacyEndcallCompatibleMessagePrefix"

#define ARM ar::rtm

namespace ar {
  namespace rtm {
      
      /**
       @brief Error codes related to Initialization.
       */
      enum INIT_ERR_CODE {
          
          /**
           0: Initialization succeeds.
           */
          INIT_ERR_OK = 0,
          
          /**
           1: A common failure occurs during initialization.
           */
          INIT_ERR_FAILURE = 1,
          
          /**
           2: The SDK is already initialized.
           */
          INIT_ERR_ALREADY_INITIALIZED = 2,
          
          /**
           3: The App ID is invalid.
           */
          INIT_ERR_INVALID_APP_ID = 3,
          
          /**
           4: The event handler is empty.
           */
          INIT_ERR_INVALID_ARGUMENT = 4,
      };
     
      
    /**
     @brief Error codes related to login.
     */
    enum LOGIN_ERR_CODE {
      
      /**
       0: The method call succeeds, or login succeeds. 
       */
      LOGIN_ERR_OK = 0,
        
      /**
       1: Login fails. The reason is unknown.
       */
      LOGIN_ERR_UNKNOWN = 1,
      
      /**
       2: Login is rejected by the server.
       */
      LOGIN_ERR_REJECTED = 2, // Occurs when not initialized or server reject
        
      /**
       3: Invalid login argument.
       */
      LOGIN_ERR_INVALID_ARGUMENT = 3,
        
      /**
       4: The App ID is invalid.
       */
      LOGIN_ERR_INVALID_APP_ID = 4,
        
      /**
       5: The token is invalid.
       */
      LOGIN_ERR_INVALID_TOKEN = 5,
       
      /**
       6: The token has expired, and hence login is rejected.
       */
      LOGIN_ERR_TOKEN_EXPIRED = 6,
       
      /**
       7: Unauthorized login.
       */
      LOGIN_ERR_NOT_AUTHORIZED = 7,
       
      /**
       8: The user has already logged in or is logging in the AR RTM system, or the user has not called the \ref ar::rtm::IRtmService::logout "logout" method to leave the \ref ar::rtm::CONNECTION_STATE_ABORTED "CONNECTION_STATE_ABORTED" state.
       */
      LOGIN_ERR_ALREADY_LOGGED_IN = 8,
       
      /**
       9: The login times out. The current timeout is set as six seconds. 
       */
      LOGIN_ERR_TIMEOUT = 9,
       
      /**
       10: The call frequency of the \ref ar::rtm::IRtmService::login "login" method exceeds the limit of two queries per second.
       */
      LOGIN_ERR_TOO_OFTEN = 10,
        
      /**
       101: \ref ar::rtm::IRtmService "IRtmService" is not initialized.
      */
      LOGIN_ERR_NOT_INITIALIZED = 101,
    };
      
    /**
     @brief Error codes related to logout.
     */
    enum LOGOUT_ERR_CODE {
       
      /**
       0: The method call succeeds, or logout succeeds.
       */
      LOGOUT_ERR_OK = 0,
      
      /**
       1: **RESERVED FOR FUTURE USE**
       */
      LOGOUT_ERR_REJECTED = 1,
        
      /**
       101: \ref ar::rtm::IRtmService "IRtmService" is not initialized.
       */
      LOGOUT_ERR_NOT_INITIALIZED = 101,
        
      /**
       102: The user does not call the \ref ar::rtm::IRtmService::login "login" method, or the method call of \ref ar::rtm::IRtmService::login "login" does not succeed before the user logs out of the AR RTM system.
       */
      LOGOUT_ERR_USER_NOT_LOGGED_IN = 102,
    };

    /**
     @brief Error codes related to renewing the RTM Token.
     */
    enum RENEW_TOKEN_ERR_CODE {
        
      /**
       0: The method call succeeds, or the renewing operation succeeds. 
       */
      RENEW_TOKEN_ERR_OK = 0,
        
      /**
       1: Common failure. The user fails to renew the token.
       */
      RENEW_TOKEN_ERR_FAILURE = 1,
 
      /**
       2: The method call fails. The argument is invalid. 
       */
      RENEW_TOKEN_ERR_INVALID_ARGUMENT = 2,
        
      /**
       3: **RESERVED FOR FUTURE USE**
       */
      RENEW_TOKEN_ERR_REJECTED = 3,
 
      /**
       4: The method call frequency exceeds the limit of two queries per second.
       */
      RENEW_TOKEN_ERR_TOO_OFTEN = 4,
  
      /**
       5: The token has expired.
       */
      RENEW_TOKEN_ERR_TOKEN_EXPIRED = 5,
  
      /**
       6: The token is invalid.
       */
      RENEW_TOKEN_ERR_INVALID_TOKEN = 6,
         
      /**
       101: \ref ar::rtm::IRtmService "IRtmService" is not initialized.
       */
      RENEW_TOKEN_ERR_NOT_INITIALIZED = 101,
        
      /**
       102: The user does not call the \ref ar::rtm::IRtmService::login "login" method, or the method call of \ref ar::rtm::IRtmService::login "login" does not succeed before renewing the token.
       */
      RENEW_TOKEN_ERR_USER_NOT_LOGGED_IN = 102,
    };
 
    /**
     @brief Connection states between the SDK and the AR RTM system.
     */
    enum CONNECTION_STATE {
        
      /**
       1: The initial state. The SDK is disconnected from the AR RTM system.
       
       When the user calls the \ref ar::rtm::IRtmService::login "login" method, the SDK starts to log in the AR RTM system, triggers the \ref ar::rtm::IRtmServiceEventHandler::onConnectionStateChanged "onConnectionStateChanged" callback, and switches to the \ref ar::rtm::CONNECTION_STATE_CONNECTING "CONNECTION_STATE_CONNECTING" state.
       */
      CONNECTION_STATE_DISCONNECTED = 1,
        
      /**
       2: The SDK is logging in the AR RTM system.

       - If the user successfully logs in the AR RTM system and receives the \ref ar::rtm::IRtmServiceEventHandler::onLoginSuccess "onLoginSuccess" callback, the SDK triggers the \ref ar::rtm::IRtmServiceEventHandler::onConnectionStateChanged "onConnectionStateChanged" callback and switches to the \ref ar::rtm::CONNECTION_STATE_CONNECTED "CONNECTION_STATE_CONNECTED" state.
       - If the user fails to login the AR RTM system and receives the \ref ar::rtm::IRtmServiceEventHandler::onLoginFailure "onLoginFailure" callback, the SDK triggers the \ref ar::rtm::IRtmServiceEventHandler::onConnectionStateChanged "onConnectionStateChanged" callback and switches to the \ref ar::rtm::CONNECTION_STATE_DISCONNECTED "CONNECTION_STATE_DISCONNECTED" state.
       */
      CONNECTION_STATE_CONNECTING = 2,
        
      /**
       3: The SDK has logged in the AR RTM system.

       - If the connection state between the SDK and the AR RTM system is interrupted because of network issues, the SDK triggers the \ref ar::rtm::IRtmServiceEventHandler::onConnectionStateChanged "onConnectionStateChanged" callback and switches to the \ref ar::rtm::CONNECTION_STATE_RECONNECTING "CONNECTION_STATE_RECONNECTING" state.
       - If the login is banned by the server because, for example, another instance logs in the AR RTM system with the same user ID, the \ref ar::rtm::IRtmServiceEventHandler::onConnectionStateChanged "onConnectionStateChanged" callback is triggered, and the SDK is switched to the \ref ar::rtm::CONNECTION_STATE_ABORTED "CONNECTION_STATE_ABORTED" state.
       - If the user calls the \ref ar::rtm::IRtmService::logout "logout" method to log out of the AR RTM system and receives the \ref ar::rtm::IRtmServiceEventHandler::onLogout "onLogout" callback (error code = `LOGOUT_ERR_OK`), the SDK triggers the \ref ar::rtm::IRtmServiceEventHandler::onConnectionStateChanged "onConnectionStateChanged" callback and switches to the \ref ar::rtm::CONNECTION_STATE_DISCONNECTED "CONNECTION_STATE_DISCONNECTED" state.
       */
      CONNECTION_STATE_CONNECTED = 3,
        
      /**
       4: The connection state between the SDK and the AR RTM system is interrupted due to network issues, and the SDK keeps re-logging in the AR RTM system.

       - If the SDK successfully re-logs in the AR RTM system, the SDK triggers the \ref ar::rtm::IRtmServiceEventHandler::onConnectionStateChanged "onConnectionStateChanged" callback and switches to the \ref ar::rtm::CONNECTION_STATE_CONNECTED "CONNECTION_STATE_CONNECTED" state. The SDK automatically adds the user back to the channel(s) he or she was in when the connection was interrupted, and synchronizes the local user's attributes with the server. 
       - If the SDK cannot re-log in the AR RTM system, it stays in the \ref ar::rtm::CONNECTION_STATE_RECONNECTING "CONNECTION_STATE_RECONNECTING" state and keeps re-logging in the system.
       */
      CONNECTION_STATE_RECONNECTING = 4,
        
      /**
       5: The SDK gives up logging in the AR RTM system, mainly because another instance has logged in the AR RTM system with the same user ID.
       
       You must call the \ref ar::rtm::IRtmService::logout "logout" method to leave this state before calling the \ref ar::rtm::IRtmService::login "login" method again.
       */
      CONNECTION_STATE_ABORTED = 5,
    };

    /**
     @brief Reasons for a connection state change.
     */
    enum CONNECTION_CHANGE_REASON {
        
      /**
       1: The SDK is logging in the AR RTM system.
       */
      CONNECTION_CHANGE_REASON_LOGIN = 1,
        
      /**
       2: The SDK has logged in the AR RTM system.
       */
      CONNECTION_CHANGE_REASON_LOGIN_SUCCESS = 2,
        
      /**
       3: The SDK fails to log in the AR RTM system.
       */
      CONNECTION_CHANGE_REASON_LOGIN_FAILURE = 3,
        
      /**
       4: The SDK fails to log in the AR RTM system within six seconds and gives up.
       */
      CONNECTION_CHANGE_REASON_LOGIN_TIMEOUT = 4,
        
      /**
       5: The connection between the SDK and the AR RTM system is interrupted. The system defines an interruption when the SDK loses connection with the AR RTM system for network reasons and cannot recover in four seconds.
       */
      CONNECTION_CHANGE_REASON_INTERRUPTED = 5,
        
      /**
       6: The user has called the \ref ar::rtm::IRtmService::logout "logout" method to log out of the AR RTM system.
       */
      CONNECTION_CHANGE_REASON_LOGOUT = 6,
        
      /**
       7: The SDK login to the AR RTM system is banned by AR.
       */
      CONNECTION_CHANGE_REASON_BANNED_BY_SERVER = 7,
        
      /**
       8: Another user is logging in the AR RTM system with the same User ID.
       */
      CONNECTION_CHANGE_REASON_REMOTE_LOGIN = 8,
    };

    /**
     @brief Error codes related to sending a peer-to-peer message.
     */
    enum PEER_MESSAGE_ERR_CODE {
        
      /**
       0: The method call succeeds, or the specified user receives the peer-to-peer message.
       */
      PEER_MESSAGE_ERR_OK = 0,

      /**
       1: The sender fails to send the peer-to-peer message.
       */
      PEER_MESSAGE_ERR_FAILURE = 1,
        
      /**
       2: A timeout occurs when sending the peer-to-peer message. The current timeout is set as 10 seconds. Possible reasons: The user is in the \ref ar::rtm::CONNECTION_STATE_ABORTED "CONNECTION_STATE_ABORTED" or \ref ar::rtm::CONNECTION_STATE_RECONNECTING "CONNECTION_STATE_RECONNECTING" state.
       */
      PEER_MESSAGE_ERR_SENT_TIMEOUT = 2,
        
      /**
       3: The specified user is offline and does not receive this peer-to-peer message.
       */
      PEER_MESSAGE_ERR_PEER_UNREACHABLE = 3,
        
     /**
      4: The receiver is offline and does not receive this offline peer-to-peer message, but the server has cached it and will re-send it once he/she is back online.
      */
      PEER_MESSAGE_ERR_CACHED_BY_SERVER = 4,
        
     /**
      5: The method call frequency exceeds the limit of 60 queries per second (channel and peer messages taken together).
      */
      PEER_MESSAGE_ERR_TOO_OFTEN = 5,
        
     /**
      6: The user ID is invalid.
      */
      PEER_MESSAGE_ERR_INVALID_USERID = 6,
        
     /**
      7: The message is null or exceeds 32 KB in length.       
	  */
      PEER_MESSAGE_ERR_INVALID_MESSAGE = 7,
      
      /**
       8: The message receiver‘s SDK is of an earlier version and hence cannot recognize this message.
       */
      PEER_MESSAGE_ERR_IMCOMPATIBLE_MESSAGE = 8,
        
     /**
      101: \ref ar::rtm::IRtmService "IRtmService" is not initialized.
      */
      PEER_MESSAGE_ERR_NOT_INITIALIZED = 101,
    
     /**
      102: The sender does not call the \ref ar::rtm::IRtmService::login "login" method, or the method call of \ref ar::rtm::IRtmService::login "login" does not succeed before sending the peer-to-peer message.
      */
      PEER_MESSAGE_ERR_USER_NOT_LOGGED_IN = 102,
    };

    /**
     @brief Error codes related to joining a channel.
     */
    enum JOIN_CHANNEL_ERR {
        
      /**
       0: The method call succeeds, or the user joins the channel successfully.
       */
      JOIN_CHANNEL_ERR_OK = 0,

      /**
       1: Common failure. The user fails to join the channel.
       */
      JOIN_CHANNEL_ERR_FAILURE = 1,
        
      /**
       2: **RESERVED FOR FUTURE USE**
       */
      JOIN_CHANNEL_ERR_REJECTED = 2, // Usually occurs when the user is already in the channel
        
      /**
       3: The user fails to join the channel because the argument is invalid.
       */
      JOIN_CHANNEL_ERR_INVALID_ARGUMENT = 3,
        
      /**
       4: A timeout occurs when joining the channel. The current timeout is set as five seconds. Possible reasons: The user is in the \ref ar::rtm::CONNECTION_STATE_ABORTED "CONNECTION_STATE_ABORTED" or \ref ar::rtm::CONNECTION_STATE_RECONNECTING "CONNECTION_STATE_RECONNECTING" state.
       */
      JOIN_CHANNEL_TIMEOUT = 4,
        
     /**
      5: The number of the RTM channels you are in exceeds the limit of 20.
      */
      JOIN_CHANNEL_ERR_EXCEED_LIMIT = 5,
        
     /**
      6: The user is joining or has joined the channel.
      */
      JOIN_CHANNEL_ERR_ALREADY_JOINED = 6,
        
      /**
      7: The method call frequency exceeds 50 queries every three seconds.
      */
      JOIN_CHANNEL_ERR_TOO_OFTEN = 7,

      /**
       8: The frequency of joining the same channel exceeds two times every five seconds.
       */
      JOIN_CHANNEL_ERR_JOIN_SAME_CHANNEL_TOO_OFTEN = 8,
        
     /**
      101: \ref ar::rtm::IRtmService "IRtmService" is not initialized.
      */
      JOIN_CHANNEL_ERR_NOT_INITIALIZED = 101,
        
     /**
      102: The user does not call the \ref ar::rtm::IRtmService::login "login" method, or the method call of \ref ar::rtm::IRtmService::login "login" does not succeed before joining the channel.
      */
      JOIN_CHANNEL_ERR_USER_NOT_LOGGED_IN = 102,
    };

    /**
     @brief Error codes related to leaving a channel.
     */
    enum LEAVE_CHANNEL_ERR {
      
      /**
       0: The method call succeeds, or the user leaves the channel successfully.
       */
      LEAVE_CHANNEL_ERR_OK = 0,
        
      /**
       1: Common failure. The user fails to leave the channel.
       */
      LEAVE_CHANNEL_ERR_FAILURE = 1,
        
      /**
       2: **RESERVED FOR FUTURE USE**
       */
      LEAVE_CHANNEL_ERR_REJECTED = 2,
        
      /**
       3: The user is not in the channel.
       */
      LEAVE_CHANNEL_ERR_NOT_IN_CHANNEL = 3,
        
      /**
       101: \ref ar::rtm::IRtmService "IRtmService" is not initialized.
       */
      LEAVE_CHANNEL_ERR_NOT_INITIALIZED = 101,
    
      /**
       102: The user does not call the \ref ar::rtm::IRtmService::login "login" method, or the method call of \ref ar::rtm::IRtmService::login "login" does not succeed before calling the \ref ar::rtm::IChannel::leave "leave" method.
       */
      LEAVE_CHANNEL_ERR_USER_NOT_LOGGED_IN = 102,
    };
      
    /**
     @brief Reasons why a user leaves the channel.
     */
    enum LEAVE_CHANNEL_REASON {
        
      /**
       1: The user has quit the call.
       */
      LEAVE_CHANNEL_REASON_QUIT = 1,
        
      /**
       2: The user is banned by the server.
       */
      LEAVE_CHANNEL_REASON_KICKED = 2,
    };

    /**
     @brief Error codes related to sending a channel message.
     */
    enum CHANNEL_MESSAGE_ERR_CODE {
        
      /**
       0: The method call succeeds, or the server receives the channel message.
       */
      CHANNEL_MESSAGE_ERR_OK = 0,

      /**
       1: Common failure. The user fails to send the channel message.
       */
      CHANNEL_MESSAGE_ERR_FAILURE = 1,
             
      /**
       2: The SDK does not receive a response from the server in 10 seconds. The current timeout is set as 10 seconds. Possible reasons: The user is in the \ref ar::rtm::CONNECTION_STATE_ABORTED "CONNECTION_STATE_ABORTED" or \ref ar::rtm::CONNECTION_STATE_RECONNECTING "CONNECTION_STATE_RECONNECTING" state.
       */
      CHANNEL_MESSAGE_ERR_SENT_TIMEOUT = 2,
        
      /**
       3: The method call frequency exceeds the limit of 60 queries per second (channel and peer messages taken together).
       */
      CHANNEL_MESSAGE_ERR_TOO_OFTEN = 3,
        
      /**
       4: The message is null or exceeds 32 KB in length.
       */
      CHANNEL_MESSAGE_ERR_INVALID_MESSAGE = 4,
        
      /**
       101: \ref ar::rtm::IRtmService "IRtmService" is not initialized.
       */
      CHANNEL_MESSAGE_ERR_NOT_INITIALIZED = 101,
    
      /**
       102: The user does not call the \ref ar::rtm::IRtmService::login "login" method, or the method call of \ref ar::rtm::IRtmService::login "login" does not succeed before sending out a channel message.
       */
      CHANNEL_MESSAGE_ERR_USER_NOT_LOGGED_IN = 102,
    };

    /**
     @brief Error codes related to retrieving a channel member list.
     */
    enum GET_MEMBERS_ERR {
  
      /**
       0: The method call succeeds, or the operation succeeds.
       */
      GET_MEMBERS_ERR_OK = 0,
    
      /**
       1: Common failure. The user fails to retrieve a member list of the channel.
       */
      GET_MEMBERS_ERR_FAILURE = 1,
        
      /**
       2: **RESERVED FOR FUTURE USE**
       */
      GET_MEMBERS_ERR_REJECTED = 2,
        
      /**
       3: A timeout occurs when retrieving a member list of the channel. The current timeout is set as five seconds. Possible reasons: The user is in the \ref ar::rtm::CONNECTION_STATE_ABORTED "CONNECTION_STATE_ABORTED" or \ref ar::rtm::CONNECTION_STATE_RECONNECTING "CONNECTION_STATE_RECONNECTING" state.
       */
      GET_MEMBERS_ERR_TIMEOUT = 3,
        
      /**
       4: The method call frequency exceeds the limit of five queries every two seconds.
       */
      GET_MEMBERS_ERR_TOO_OFTEN = 4,
    
      /**
       5: The user is not in channel.
       */
      GET_MEMBERS_ERR_NOT_IN_CHANNEL = 5,
        
      /**
       101: \ref ar::rtm::IRtmService "IRtmService" is not initialized.
       */
      GET_MEMBERS_ERR_NOT_INITIALIZED = 101,
        
      /**
       102: The user does not call the \ref ar::rtm::IRtmService::login "login" method, or the method call of \ref ar::rtm::IRtmService::login "login" does not succeed before retrieving a member list.
       */
      GET_MEMBERS_ERR_USER_NOT_LOGGED_IN = 102,
    };

    /**
     @brief Error codes related to querying the online status of the specified peer(s).
     */
    enum QUERY_PEERS_ONLINE_STATUS_ERR {
      
      /**
       0: The method call succeeds, or the operation succeeds.
       */
      QUERY_PEERS_ONLINE_STATUS_ERR_OK = 0,

      /**
       1: Common failure. The user fails to query the online status of the specified peer(s).
       */
      QUERY_PEERS_ONLINE_STATUS_ERR_FAILURE = 1,
        
      /**
       2: The method call fails. The argument is invalid. 
       */
      QUERY_PEERS_ONLINE_STATUS_ERR_INVALID_ARGUMENT = 2,
        
      /**
       3: **RESERVED FOR FUTURE USE**
       */
      QUERY_PEERS_ONLINE_STATUS_ERR_REJECTED = 3,
        
      /**
       4: The SDK fails to receive a response from the server in 10 seconds. The current timeout is set as 10 seconds. Possible reasons: The user is in the \ref ar::rtm::CONNECTION_STATE_ABORTED "CONNECTION_STATE_ABORTED" or \ref ar::rtm::CONNECTION_STATE_RECONNECTING "CONNECTION_STATE_RECONNECTING" state.
       */
      QUERY_PEERS_ONLINE_STATUS_ERR_TIMEOUT = 4,
        
      /**
       5: The method call frequency exceeds the limit of 10 queries every five seconds.
       */
      QUERY_PEERS_ONLINE_STATUS_ERR_TOO_OFTEN = 5,
        
      /**
       101: \ref ar::rtm::IRtmService "IRtmService" is not initialized.
       */
      QUERY_PEERS_ONLINE_STATUS_ERR_NOT_INITIALIZED = 101,
    
      /**
       102: The user does not call the \ref ar::rtm::IRtmService::login "login" method, or the method call of \ref ar::rtm::IRtmService::login "login" does not succeed before querying the online status.
       */
      QUERY_PEERS_ONLINE_STATUS_ERR_USER_NOT_LOGGED_IN = 102,
    };

    /**
     @brief The online states of a peer.
     */
    enum PEER_ONLINE_STATE {

      /**
       0: The peer is online (the SDK has logged in the AR RTM system).
       */
      PEER_ONLINE_STATE_ONLINE = 0,

      /**
       1: The peer is temporarily unreachable (the server has not received a packet from the SDK for more than six seconds).
       */
      PEER_ONLINE_STATE_UNREACHABLE = 1,

      /**
       2: The peer is offline (the SDK has not logged in the AR RTM system, or it has logged out of the system, or the server has not received a packet from the SDK for more than 30 seconds).
       */
      PEER_ONLINE_STATE_OFFLINE = 2,
    };
      
    /**
     @brief Subscription types.
     */
    enum PEER_SUBSCRIPTION_OPTION {
      /**
       0: Takes out a subscription to the online status of specified user(s).
       */
      PEER_SUBSCRIPTION_OPTION_ONLINE_STATUS = 0,
    };

    /**
     @brief Error codes related to subscribing to or unsubscribing from the status of specified peer(s).
     */
    enum PEER_SUBSCRIPTION_STATUS_ERR {

      /**
       0: The method call succeeds, or the operation succeeds.
       */
      PEER_SUBSCRIPTION_STATUS_ERR_OK = 0,

      /**
       1: Common failure. The user fails to subscribe to or unsubscribe from the status of the specified peer(s).
       */
      PEER_SUBSCRIPTION_STATUS_ERR_FAILURE = 1,

      /**
       2: The method call fails. The argument is invalid.
       */
      PEER_SUBSCRIPTION_STATUS_ERR_INVALID_ARGUMENT = 2,

      /**
       3: **RESERVED FOR FUTURE USE**
       */
      PEER_SUBSCRIPTION_STATUS_ERR_REJECTED = 3,

      /**
       4: The SDK fails to receive a response from the server within 10 seconds. The current timeout is set as 10 seconds. Possible reasons: The user is in the \ref ar::rtm::CONNECTION_STATE_ABORTED "CONNECTION_STATE_ABORTED" or \ref ar::rtm::CONNECTION_STATE_RECONNECTING "CONNECTION_STATE_RECONNECTING" state.
       */
      PEER_SUBSCRIPTION_STATUS_ERR_TIMEOUT = 4,

      /**
       5: The method call frequency exceeds the limit of 10 queries every five seconds.
       */
      PEER_SUBSCRIPTION_STATUS_ERR_TOO_OFTEN = 5,

      /**
       6: The number of peers, to whom you subscribe, exceeds the limit of 512.
       */
      PEER_SUBSCRIPTION_STATUS_ERR_OVERFLOW = 6,

      /**
       101: \ref ar::rtm::IRtmService "IRtmService" is not initialized.
       */
      PEER_SUBSCRIPTION_STATUS_ERR_NOT_INITIALIZED = 101,

      /**
       102: The user does not call the \ref ar::rtm::IRtmService::login "login" method, or the method call of \ref ar::rtm::IRtmService::login "login" does not succeed before this operation.
       */
      PEER_SUBSCRIPTION_STATUS_ERR_USER_NOT_LOGGED_IN = 102,
    };

    /**
     @brief Error codes related to getting a list of the peer(s) by suscription option type.
     */
    enum QUERY_PEERS_BY_SUBSCRIPTION_OPTION_ERR {

      /**
       0: The method call succeeds, or the operation succeeds.
       */
      QUERY_PEERS_BY_SUBSCRIPTION_OPTION_ERR_OK = 0,

      /**
       1: Common failure. The user fails to query peer(s) by subscription option type.
       */
      QUERY_PEERS_BY_SUBSCRIPTION_OPTION_ERR_FAILURE = 1,

      /**
       2: The SDK fails to receive a response from the server within 5 seconds. The current timeout is set as 5 seconds. Possible reasons: The user is in the \ref ar::rtm::CONNECTION_STATE_ABORTED "CONNECTION_STATE_ABORTED" or \ref ar::rtm::CONNECTION_STATE_RECONNECTING "CONNECTION_STATE_RECONNECTING" state.
       */
      QUERY_PEERS_BY_SUBSCRIPTION_OPTION_ERR_TIMEOUT = 2,

      /**
       3: The method call frequency exceeds the limit of 10 queries every five seconds.
       */
      QUERY_PEERS_BY_SUBSCRIPTION_OPTION_ERR_TOO_OFTEN = 3,

      /**
       101: \ref ar::rtm::IRtmService "IRtmService" is not initialized.
       */
      QUERY_PEERS_BY_SUBSCRIPTION_OPTION_ERR_NOT_INITIALIZED = 101,

      /**
       102: The user does not call the \ref ar::rtm::IRtmService::login "login" method, or the method call of \ref ar::rtm::IRtmService::login "login" does not succeed before the query.
       */
      QUERY_PEERS_BY_SUBSCRIPTION_OPTION_ERR_USER_NOT_LOGGED_IN = 102,
    };
      
    /**
     @brief Error codes related to attrubute operations.
     */
    enum ATTRIBUTE_OPERATION_ERR {
        
        /**
         0: The method call succeeds, or the attribute operation succeeds.
         */
        ATTRIBUTE_OPERATION_ERR_OK = 0,
        
        /**
         1: @deprecated
         */
        ATTRIBUTE_OPERATION_ERR_NOT_READY = 1,
        
        /**
         2: Common failure. The attribute operation fails.
         */
        ATTRIBUTE_OPERATION_ERR_FAILURE = 2,
        
        /**
         3: The argument you put for this attribute operation is invalid. For example, you cannot set a user or channel attribute as "".
         */
        ATTRIBUTE_OPERATION_ERR_INVALID_ARGUMENT = 3,
        
        /**
         4: The attribute size exceeds the limit.
         
         - For user attribute operations: The user's overall attribute size would exceed the limit of 16 KB, one of the user's attributes would exceeds 8 KB in size, or the number of this user's attributes would exceed 32 after this attribute operation.
         - For channel attribute operations: The channel's overall attribute size would exceed the limit of 32 KB, one of the channel attributes would exceed 8 KB in size, or the number of this channel's attributes would exceed 32 after this attribute operation.
         */
        ATTRIBUTE_OPERATION_ERR_SIZE_OVERFLOW = 4,
        
        /**
         5: The method call frequency exceeds the limit.
         
         - For \ref ar::rtm::IRtmService::setLocalUserAttributes "setLocalUserAttributes", \ref ar::rtm::IRtmService::addOrUpdateLocalUserAttributes "addOrUpdateLocalUserAttributes", \ref ar::rtm::IRtmService::deleteLocalUserAttributesByKeys "deleteLocalUserAttributesByKeys" and \ref ar::rtm::IRtmService::clearLocalUserAttributes "clearLocalUserAttributes" taken together: the call frequency limit is 10 queries every five seconds.
         - For \ref ar::rtm::IRtmService::getUserAttributes "getUserAttributes" and \ref ar::rtm::IRtmService::getUserAttributesByKeys "getUserAttributesByKeys" taken together, the call frequency limit is 40 queries every five seconds.
         - For \ref ar::rtm::IRtmService::setChannelAttributes "setChannelAttributes", \ref ar::rtm::IRtmService::addOrUpdateChannelAttributes "addOrUpdateChannelAttributes", \ref ar::rtm::IRtmService::deleteChannelAttributesByKeys "deleteChannelAttributesByKeys" and \ref ar::rtm::IRtmService::clearChannelAttributes "clearChannelAttributes" taken together: the call frequency limit is 10 queries every five seconds.
         - For \ref ar::rtm::IRtmService::getChannelAttributes "getChannelAttributes" and \ref ar::rtm::IRtmService::getChannelAttributesByKeys "getChannelAttributesByKeys" taken together, the call frequency limit is 10 queries every five seconds.
         */
        ATTRIBUTE_OPERATION_ERR_TOO_OFTEN = 5,
        
        /**
         6: The specified user is not found, either because the user is offline or because the user does not exist.
         */
        ATTRIBUTE_OPERATION_ERR_USER_NOT_FOUND = 6,
        
        /**
         7: A timeout occurs during the attribute operation. The current timeout is set as five seconds. Possible reasons: The user is in the \ref ar::rtm::CONNECTION_STATE_ABORTED "CONNECTION_STATE_ABORTED" or \ref ar::rtm::CONNECTION_STATE_RECONNECTING "CONNECTION_STATE_RECONNECTING" state.
         */
        ATTRIBUTE_OPERATION_ERR_TIMEOUT = 7,
        
        /**
         101: \ref ar::rtm::IRtmService "IRtmService" is not initialized.
         */
        ATTRIBUTE_OPERATION_ERR_NOT_INITIALIZED = 101,
        
        /**
         102: The user does not call the \ref ar::rtm::IRtmService::login "login" method, or the method call of \ref ar::rtm::IRtmService::login "login" does not succeed before the attribute operation.
         */
        ATTRIBUTE_OPERATION_ERR_USER_NOT_LOGGED_IN = 102,
    };
     
     /**
      @brief Error codes related to retrieving the channel member count of specified channel(s).
      */
     enum GET_CHANNEL_MEMBER_COUNT_ERR_CODE {
         
        /**
         0: The method call succeeds, or the operation succeeds. 
         */
        GET_CHANNEL_MEMBER_COUNT_ERR_OK = 0,
         
        /**
         1: Unknown common failure. 
         */
        GET_CHANNEL_MEMBER_COUNT_ERR_FAILURE = 1,
        
        /**
         2: One or several of your channel IDs is invalid, or @p channelCount &lt; 0.
         */
        GET_CHANNEL_MEMBER_COUNT_ERR_INVALID_ARGUMENT = 2,
        
        /**
         3: The method call frequency exceeds the limit of one query per second.
         */
        GET_CHANNEL_MEMBER_COUNT_ERR_TOO_OFTEN = 3,
         
        /**
         4: A timeout occurs during this operation. The current timeout is set as five seconds.
         */
        GET_CHANNEL_MEMBER_COUNT_ERR_TIMEOUT = 4,
        
        /**
         5:@p channelCount is greater than 32. 
         */
        GET_CHANNEL_MEMBER_COUNT_ERR_EXCEED_LIMIT = 5,
         
        /**
         101: \ref ar::rtm::IRtmService "IRtmService" is not initialized.
         */
        GET_CHANNEL_MEMBER_COUNT_ERR_NOT_INITIALIZED = 101,
        
        /**
         102: The user does not call the \ref ar::rtm::IRtmService::login "login" method, or the method call of \ref ar::rtm::IRtmService::login "login" does not succeed before this operation.
         */
        GET_CHANNEL_MEMBER_COUNT_ERR_USER_NOT_LOGGED_IN = 102,
     };
      
      /**
       @brief Error codes related to downloading a file or image.
       */
      enum DOWNLOAD_MEDIA_ERR_CODE {
          /**
           0: The method call succeeds, or the operation succeeds.
           */
          DOWNLOAD_MEDIA_ERR_OK = 0,
          
          /**
           1: Unknown common failure.
           */
          DOWNLOAD_MEDIA_ERR_FAILURE = 1,
          
          /**
           2: An argument you put is invalid. For example, `mediaId` is in the wrong format or `filePath` is set as `null`.
           */
          DOWNLOAD_MEDIA_ERR_INVALID_ARGUMENT = 2,
          /**
           3: A timeout occurs. The current timeout is set as 120 seconds. The SDK assumes that a timeout occurs if it has not detected any file transmission between the SDK and the file server for 120 seconds.
           */
          DOWNLOAD_MEDIA_ERR_TIMEOUT = 3,
          
          /**
           4: The file or image to download does not exist, either because the media ID you input is incorrect or because the validity of the media ID has expired. 
           */
          DOWNLOAD_MEDIA_ERR_NOT_EXIST = 4,
          
          /**
           5: You have exceeded the upper limit for file download. You can initiate a maximum of nine file download or upload tasks at the same time (download and upload tasks count together).
           */
          DOWNLOAD_MEDIA_ERR_CONCURRENCY_LIMIT_EXCEEDED = 5,

          /**
           6: The file or image download task is aborted for either of the following reasons:

           - The user is in the \ref ar::rtm::CONNECTION_STATE_ABORTED "CONNECTION_STATE_ABORTED" state. 
           - The user has cancelled the download task.
           */
          DOWNLOAD_MEDIA_ERR_INTERRUPTED = 6,
          
          /**
           101: \ref ar::rtm::IRtmService "IRtmService" is not initialized.
           */
          DOWNLOAD_MEDIA_ERR_NOT_INITIALIZED = 101,
          
          /**
           102: The user does not call the \ref ar::rtm::IRtmService::login "login" method, or the method call of \ref ar::rtm::IRtmService::login "login" does not succeed before this operation.
           */
          DOWNLOAD_MEDIA_ERR_NOT_LOGGED_IN = 102,
      };
      
      
      /**
       @brief Error codes related to uploading a file or image.
       */
      enum UPLOAD_MEDIA_ERR_CODE {
          /**
           0: The method call succeeds, or the operation succeeds.
           */
          UPLOAD_MEDIA_ERR_OK = 0,
          
          /**
           1: Unknown common failure.
           */
          UPLOAD_MEDIA_ERR_FAILURE = 1,
          
          /**
           2: The argument you put is invalid. For example, `mediaId` is in the wrong format. 
           */
          UPLOAD_MEDIA_ERR_INVALID_ARGUMENT = 2,
          
          /**
           3: A timeout occurs. The current timeout is set as 120 seconds. The SDK assumes that a timeout occurs if it has not detected any file transmission between the SDK and the file server for 120 seconds.
           */
          UPLOAD_MEDIA_ERR_TIMEOUT = 3,
          
          /**
           4: The size of the file or image to upload exceeds 30 MB.
           */
          UPLOAD_MEDIA_ERR_SIZE_OVERFLOW = 4,
          /**
           5: You have exceeded the upper limit for file upload. You can initiate a maximum of nine file upload or download tasks at the same time (upload and download tasks count together).
           */
          UPLOAD_MEDIA_ERR_CONCURRENCY_LIMIT_EXCEEDED = 5,
          /**
           6: The file or image upload task is aborted for either of the following reasons:
           
           - The user in the \ref ar::rtm::CONNECTION_STATE_ABORTED "CONNECTION_STATE_ABORTED" state. 
           - The user has cancelled the upload task.
           */
          UPLOAD_MEDIA_ERR_INTERRUPTED = 6,
          
          /**
           101: \ref ar::rtm::IRtmService "IRtmService" is not initialized.
           */
          UPLOAD_MEDIA_ERR_NOT_INITIALIZED = 101,
          
          /**
           102: The user does not call the \ref ar::rtm::IRtmService::login "login" method, or the method call of \ref ar::rtm::IRtmService::login "login" does not succeed before this operation.
           */
          UPLOAD_MEDIA_ERR_NOT_LOGGED_IN = 102,
      };
      
      /**
       @brief Error codes related to cancelling a download task or cancelling an upload task.
       */
      enum CANCEL_MEDIA_ERR_CODE {
          /**
           0: The method call succeeds, or the operation succeeds.
           */
          CANCEL_MEDIA_ERR_OK = 0,
          
          /**
           1: Unknown common failure.
           */
          CANCEL_MEDIA_ERR_FAILURE = 1,
          
          /**
           2: The task to cancel does not exist. You can only cancel an ongoing download or upload task. If the download or upload task completes, the corresponding @p requestId is no longer valid. 
           */
          CANCEL_MEDIA_ERR_NOT_EXIST = 2,
          
          /**
           101: \ref ar::rtm::IRtmService "IRtmService" is not initialized.
           */
          CANCEL_MEDIA_ERR_NOT_INITIALIZED = 101,
          
          /**
           102: The user does not call the \ref ar::rtm::IRtmService::login "login" method, or the method call of \ref ar::rtm::IRtmService::login "login" does not succeed before this operation.
           */
          CANCEL_MEDIA_ERR_NOT_LOGGED_IN = 102,
      };
      
      
      
    /**
     @brief Message types.
     */
    enum MESSAGE_TYPE {
        
        /**
        0: The message type is undefined.
        */
        MESSAGE_TYPE_UNDEFINED = 0,

        /**
        1: A text message.
        */
        MESSAGE_TYPE_TEXT = 1,
        
        /**
        2: A raw message. A raw message is a binary message whose size does not exceed 32 KB. 
        */
        MESSAGE_TYPE_RAW = 2,
        
        /**
         3: A file message. The size of a file message must be less than 32 KB.
         */
        MESSAGE_TYPE_FILE = 3,
        
        /**
         4: An image message. The size of an image message must be less than 32 KB.
         */
        MESSAGE_TYPE_IMAGE = 4,
    };
     
     /**
      @brief Message sending options.
      */
    struct SendMessageOptions{
      /**
      Set the message as an offline message.

      - true: Set the message as an offline message.
      - false: (default) Do not set the message as an offline message.
       
      @note This setting applies to the peer-to-peer message only, not to the channel message. 
      */
      bool enableOfflineMessaging;
      /**
       <b>PRIVATE BETA</b> Save the message to message history.
     
       - true: Save the message to message history.
       - false: (default) Do not save the message to message history.
       */
      bool enableHistoricalMessaging;

      SendMessageOptions()
      : enableOfflineMessaging(false)

      , enableHistoricalMessaging(false)

      {}
    };
   
    /**
     @brief A data structure holding a user attribute key and its value.
     */
    struct RtmAttribute
    {
    
        /**
         User attribute name. Must be visible characters and not exceed 32 Bytes.
         */
        const char* key;
  
        /**
         Value of the user attribute. Must not exceed 8 KB.
         */
        const char* value;
    };

    /**
     @brief A data structure representing the upload ratio or download ratio.
     */
    struct MediaOperationProgress
    {
        /**
         The total size of the file or image being loaded.
         */
        long long totalSize;
        /**
         The size of the loaded part of the file or image. 
         */
        long long currentSize;
    };
      
     /**
      @brief The class for setting or getting attributes of a channel.
       */
     class IRtmChannelAttribute
     {
     protected:
        virtual ~IRtmChannelAttribute() {}
     public:
        
        /**
         Sets the key of the channel attribute.

         @param key Key of channel attribute. Must be visible characters and not exceed 32 Bytes. 
         */
        virtual void setKey(const char *key) = 0;
         
        /**
         Gets the key of the channel attribute.

         @return Key of the channel attribute.
         */
        virtual const char *getKey() const = 0;
        
        /**
         Sets the value of the channel attribute.

         @param value Value of the channel attribute. Must not exceed 8 KB in length.
         */
        virtual void setValue(const char *value) = 0;
        
        /**
         Gets the value of the channel attribute.

         @return Value of the channel attribute.
         */
        virtual const char *getValue() const = 0;
        
        /**
         Gets the User ID of the user who makes the latest update to the channel attribute.

         @return User ID of the user who makes the latest update to the channel attribute.  
         */
        virtual const char *getLastUpdateUserId() const = 0;
        
        /**
         Gets the timestamp of when the channel attribute was last updated.

         @return Timestamp of when the channel attribute was last updated in milliseconds.
         */
        virtual long long getLastUpdateTs() const = 0;
        
        /**
         Release all resources used by the \ref ar::rtm::IRtmChannelAttribute "IRtmChannelAttribute" instance. 
         */
        virtual void release() = 0;
     };
     
     /**
      @brief Data structure holding channel attribute-specific options.
      */
     struct ChannelAttributeOptions{
        
        /**
         Indicates whether or not to notify all channel members of a channel attribute change.
         
         @note This flag is valid only within the current method call.
         
         - true: Notify all channel members of a channel attribute change.
         - false: (Default) Do not notify all channel members of a channel attribute change.
         */
        bool enableNotificationToChannelMembers;
        ChannelAttributeOptions():enableNotificationToChannelMembers(false){}
     };
      
    /**
     @brief The class for setting and retrieving attributes of a channel or peer-to-peer message.
     */
    class IMessage
    {
    protected:
      virtual ~IMessage() {}
    public:
        
      /**
       Retrieves the unique ID of the message.
       
       @return The message ID.
       */
      virtual long long getMessageId() const = 0;
        
      /**
       Retrieves the message type.

       @return The message type. See #MESSAGE_TYPE. 
       */
      virtual MESSAGE_TYPE getMessageType() const = 0;
        
      /**
       Sets the content of a text message, or the text description of a raw message.
       
       @param str The text message to be set. Must not exceed 32 KB in length. If the message is a raw message, ensure that the overall size of the text description and the raw message data does not exceed 32 KB.
       */
      virtual void setText(const char *str) = 0;
        
      /**
       Retrieves the content of a text message, or the text description of a raw message.
       
       @return The content of the received text message, or the text description of the received raw message.
       */
      virtual const char *getText() const = 0;
      
      /**
       Retrieves the starting address of the raw message in the memory.
       
       @return The starting address of the raw message in the memory.
       */
      virtual const char *getRawMessageData() const = 0;
    
      /**
       Retrieves the length of the raw message.
       
       @return The length of the raw message in Bytes.
       */
      virtual int getRawMessageLength() const = 0;
      /**
       Allows the receiver to retrieve the timestamp of when the messaging server receives this message.
       
       @note
       - You can infer from the returned timestamp the *approximate* time as to when this message was sent.
       - The returned timestamp is on a millisecond time-scale. It is for demonstration purposes only, not for strict ordering of messages.
       
       
       @return The timestamp (ms) of when the messaging server receives this message.
       */
      virtual long long getServerReceivedTs() const = 0;
        
      /**
       Allows the receiver to check whether this message has been cached on the server (Applies to peer-to-peer message only).
       
       @note
       - This method returns false if a message is not cached by the server. Only if the sender sends the message as an offline message (sets \ref ar::rtm::SendMessageOptions::enableOfflineMessaging "enableOfflineMessaging" as true) when the specified user is offline, does the method return true when the user is back online.
       - For now we only cache 200 offline messages for up to seven days for each message receiver. When the number of the cached messages reaches this limit, the newest message overrides the oldest one.
       
       @return
       - true: This message has been cached on the server (the server caches this message and re-sends it to the receiver when he/she is back online).
       - false: This message has not been cached on the server.
       */
      virtual bool isOfflineMessage() const = 0;
        
      /**
       Releases all resources used by the \ref ar::rtm::IMessage "IMessage" instance.
       
       @note For the message receiver: please access and save the content of the IMessage instance when receiving the \ref ar::rtm::IChannelEventHandler::onMessageReceived "onMessageReceived" or the \ref ar::rtm::IRtmServiceEventHandler::onMessageReceivedFromPeer "onMessageReceivedFromPeer" callback. The SDK will release the IMessage instance when the callback ends.
       */
      virtual void release() = 0;
    };
    
    /**
     @brief The class for setting and retrieving attributes of a file message.
     */
    class IFileMessage : public IMessage
    {
    public:
        
        /**
         Gets the size of the uploaded file.

         @return The size of the uploaded file in bytes.
         */
        virtual long long getSize() const = 0;
        
        /**
         Gets the media ID of the uploaded file.
         
         @note
         - The media ID is automatically populated once the file is uploaded to the file server.
         - The media ID is valid for 7 days because the file server keeps all uploaded files for 7 days only. 

         @return The media ID of the uploaded file.
         */
        virtual const char* getMediaId() const = 0;
        
        /**
         Sets the thumbnail of the uploaded file.

         @param thumbnail The thumbnail of the uploaded file. Must be binary data. 
         @param length The length of the thumbnail. The size of @p thumbnail and @p fileName combined must not exceed 32 KB.
         */
        virtual void setThumbnail(const uint8_t* thumbnail, int length) = 0;
        
        /**
         Gets the thumbnail of the uploaded file.

         @return The thumbnail of the uploaded file.
         */
        virtual const char* getThumbnailData() const = 0;
        
        /**
         Gets the length of the thumbnail.

         @return The length of the thumbnail.
         */
        virtual const long long getThumbnailLength() const = 0;
        
        /**
         Sets the name of the uploaded file.

         @param fileName The name of the uploaded file. The size of @p thumbnail and @p fileName combined must not exceed 32 KB.
         */
        virtual void setFileName(const char* fileName) = 0;
        
        /**
         Gets the name of the uploaded file.

         @return The name of the uploaded file.
         */
        virtual const char* getFileName() const = 0;
    };
    
    /**
     @brief The class for setting and retrieving attributes of an image message.
     */
    class IImageMessage : public IMessage
    {
    public:
        /**
         Gets the size of the uploaded image.

         @return The size of the uploaded image in bytes.
         */
        virtual long long getSize() const = 0;
        
        /**
         Gets the media ID of the uploaded image.
         
         @note
         - The media ID is automatically populated once the image is uploaded to the file server.
         - The media ID is valid for 7 days because the file server keeps all uploaded files for 7 days only.

         @return The media ID of the uploaded image.
         */
        virtual const char* getMediaId() const = 0;
        
        /**
         Sets the thumbnail of the uploaded image.

         @param thumbnail The thumbnail of the uploaded image.
         @param length The length of the thumbnail in bytes. The size of @p thumbnail and @p fileName combined must not exceed 32 KB.
         */
        virtual void setThumbnail(const uint8_t* thumbnail, long long length) = 0;
        
        /**
         Gets the thumbnail data of the uploaded image.

         @return The thumbnail data of the uploaded image.
         */
        virtual const char* getThumbnailData() const = 0;
        
        /**
         Gets the length of the thumbnail data.

         @return The length of the thumbnail data.
         */
        virtual const long long getThumbnailLength() const = 0;
        
        /**
         Sets the file name of the uploaded image.

         @param fileName The file name of the uploaded image. The size of @p thumbnail and @p fileName combined must not exceed 32 KB.
         */
        virtual void setFileName(const char* fileName) = 0;
        
        /**
         Gets the file name of the uploaded image.

         @return The file name of the uploaded image.
         */
        virtual const char* getFileName() const = 0;
        
        /**
         Sets the width of the uploaded image.

         @note
         - If the uploaded image is in JPG, JPEG, BMP, or PNG format, the SDK automatically calculates the width and height of the image. You can call \ref ar::rtm::IImageMessage::getWidth() "getWidth" directly to get the width of the image.
         - Image width that is set by calling this method overrides the width calculated by the SDK. 

         @param width The width of the uploaded image.
         */
        virtual void setWidth(int width) = 0;
        
        /**
         Gets the width of the uploaded image.

         @note
         - If the uploaded image is in JPG, JPEG, BMP, or PNG format, the SDK automatically calculates the width and height of the image. You can call this method directly to get the width of the image.
         - Image width that is set by calling \ref ar::rtm::IImageMessage::setWidth() "setWidth" overrides the width calculated by the SDK. 

         @return The width of the uploaded image. Is 0 if the SDK does not support the format of the uploaded image.
         */
        virtual int getWidth() const = 0;
        
        /**
         Sets the height of the uploaded image.

         @note
         - If the uploaded image is in JPG, JPEG, BMP, or PNG format, the SDK automatically calculates the width and height of the image. You can call \ref ar::rtm::IImageMessage::getHeight() "getHeight" directly to get the height of the image.
         - Image height that is set by calling this method overrides the height calculated by the SDK. 

         @param height The height of the uploaded image. Is 0 if the SDK does not support the format of the uploaded image.
         */
        virtual void setHeight(int height) = 0;
        
        /**
         Gets the height of the uploaded image.

         @note
         - If the uploaded image is in JPG, JPEG, BMP, or PNG format, the SDK automatically calculates the width and height of the image. You can call this method directly to get the height of the image.
         - Image height that is set by calling \ref ar::rtm::IImageMessage::setHeight() "setHeight" overrides the height calculated by the SDK. 

         @return The height of the uploaded image.
         */
        virtual int getHeight() const = 0;
        
        /**
         Sets the width of the thumbnail.

         @note You need to work out the width of the thumbnail by yourself, because the SDK does not work out the value for you.

         @param width The width of the thumbnail.
         */
        virtual void setThumbnailWidth(int width) = 0;
        
        /**
         Gets the width of the thumbnail.

         @return The width of the thumbnail.
         */
        virtual int getThumbnailWidth() const = 0;
        
        /**
         Sets the height of the thumbnail.

         @note You need to work out the height of the thumbnail by yourself, because the SDK does not work out the value for you.

         @param height The height of the thumbnail.
         */
        virtual void setThumbnailHeight(int height) = 0;
        
        /**
         Gets the height of the thumbnail.

         @return The height of the thumbnail.
         */
        virtual int getThumbnailHeight() const = 0;
    };

    /**
     @brief The class for retrieving the attributes of a channel member.
     */
    class IChannelMember
    {
    protected:
      virtual ~IChannelMember() {}
    public:
        
      /**
       Retrieves the user ID of a user in the channel.
       
       @return User ID of a user in the channel.
       */
      virtual const char * getUserId() const = 0;
        
      /**
       Retrieves the channel ID of the user.
       
       @return Channel ID of the user.
       */
      virtual const char * getChannelId() const = 0;
        
      /**
       Releases all resources used by the \ref ar::rtm::IChannelMember "IChannelMember" instance.
       */
      virtual void release() = 0;
    };
    
    /**
     @brief The data structure holding an RTM user and the user's online status.
     */
    struct PeerOnlineStatus
    {
        
      /**
       The user ID of the peer.
       */
      const char* peerId;
        
      /**
       @deprecated Deprecated as of v1.2.0. Use \ref ar::rtm::PeerOnlineStatus::onlineState "onlineState" instead.
       
       The online status of the peer.
       
       - true: The user is online (the user has logged in the AR RTM system).
       - false: The user is offline (the user has logged out of the AR RTM system, has not logged in, or has failed to logged in).
       */
      bool isOnline;

      /**
       The online state of the peer. See #PEER_ONLINE_STATE.
       
       @note
       - The server will never return the `unreachable` state, if you <i>query</i> the online status of specified peer(s) (\ref ar::rtm::IRtmService::queryPeersOnlineStatus "queryPeersOnlineStatus"). See also: \ref ar::rtm::IRtmServiceEventHandler::onQueryPeersOnlineStatusResult "onQueryPeersOnlineStatusResult".
       - The server may return the `unreachable` state, if you <i>subscribe to</i> the online status of specified peer(s) (\ref ar::rtm::IRtmService::subscribePeersOnlineStatus "subscribePeersOnlineStatus"). See also: \ref ar::rtm::IRtmServiceEventHandler::onPeersOnlineStatusChanged "onPeersOnlineStatusChanged".
       */
      PEER_ONLINE_STATE onlineState;
    };

    /**
     @brief The data structure holding a channel ID and the current channel member count.
     */  
    struct ChannelMemberCount
    {
      /**
       The ID of the channel.
       */
      const char* channelId;
       // count of channel, 0 if channel not found.
      /**
       The current member count of the channel.

       @note @p count is 0, if a channel with the specified @p channelId is not found. 
       */ 
      int count;
    };

    /**
     @brief Callbacks for the AR RTM channel methods.
     */
    class IChannelEventHandler
    {
    public:

      virtual ~IChannelEventHandler()
      {
      }
        
      /**
       Occurs when successfully joining a channel.

       When the local user calls the \ref ar::rtm::IChannel::join "join" method and successfully joins the channel:
       - The SDK triggers this callback;
       - All remote users in the channel receive the \ref ar::rtm::IChannelEventHandler::onMemberJoined "onMemberJoined" callback.
       */
      virtual void onJoinSuccess()
      {
      }
        
      /**
       Occurs when failing to join a channel.

       The local user receives this callback when the \ref ar::rtm::IChannel::join "join" method call fails.
       
       @param errorCode The error code. See #JOIN_CHANNEL_ERR.
       */
      virtual void onJoinFailure(JOIN_CHANNEL_ERR errorCode)
      {
          (JOIN_CHANNEL_ERR) errorCode;
      }

      /**
       Returns the result of the \ref ar::rtm::IChannel::leave "leave" method call.
       
       @param errorCode The error code. See #LEAVE_CHANNEL_ERR.
       */
      virtual void onLeave(LEAVE_CHANNEL_ERR errorCode)
      {
          (LEAVE_CHANNEL_ERR) errorCode;
      }

      /**
       Occurs when receiving a channel message.

       @param userId The message sender.
       @param message The received channel message. See \ref ar::rtm::IMessage "IMessage".
       */
      virtual void onMessageReceived(const char *userId, const IMessage *message)
      {
          (const char *) userId;
          (IMessage *) message;
      }
        /**
         Occurs when receiving a channel image message.
         
         @param userId The message sender.
         @param message The received channel image message. See \ref ar::rtm::IImageMessage "IImageMessage".
         */
        virtual void onImageMessageReceived(const char *userId, const IImageMessage* message)
        {
            (IImageMessage *) message;
        }
        
        /**
         Occurs when receiving a channel file message.
         
         @param userId The message sender. 
         @param message The received channel file message. See \ref ar::rtm::IFileMessage "IFileMessage".
         */
        virtual void onFileMessageReceived(const char *userId, const IFileMessage* message)
        {
            (IFileMessage *) message;
        }

      /**
       Returns the result of the \ref ar::rtm::IChannel::sendMessage "sendMessage" method call.

       @param messageId The ID of the sent channel message.
       @param state The error codes. See #CHANNEL_MESSAGE_ERR_CODE.
       */
      virtual void onSendMessageResult(long long messageId, CHANNEL_MESSAGE_ERR_CODE state)
      {
          (long long) messageId;
          (CHANNEL_MESSAGE_ERR_CODE) state;
      }
        
      /**
       Occurs when a remote user joins the channel.

       When a remote user calls the \ref ar::rtm::IChannel::join "join" method and receives the \ref ar::rtm::IChannelEventHandler::onJoinSuccess "onJoinSuccess" callback (successfully joins the channel), the local user receives this callback.
       
       @note This callback is disabled when the number of the channel members exceeds 512.

       @param member The user joining the channel. See IChannelMember.
       */
      virtual void onMemberJoined(IChannelMember *member)
      {
          (IChannelMember *) member;
      }
        
      /**
       Occurs when a remote member leaves the channel.

       When a remote member in the channel calls the \ref ar::rtm::IChannel::leave "leave" method and receives the the \ref ar::rtm::IChannelEventHandler::onLeave "onLeave (LEAVE_CHANNEL_ERR_OK)" callback, the local user receives this callback.
       
       @note This callback is disabled when the number of the channel members exceeds 512.

       @param member The channel member that leaves the channel. See IChannelMember.
       */
      virtual void onMemberLeft(IChannelMember *member)
      {
          (IChannelMember *) member;
      }
        
      /**
       Returns the result of the \ref ar::rtm::IChannel::getMembers "getMembers" method call.
       
       When the method call succeeds, the SDK returns the member list of the channel.

       @param members The member list. See IChannelMember.
       @param userCount The number of members.
       @param errorCode Error code. See #GET_MEMBERS_ERR.
       */
      virtual void onGetMembers(IChannelMember **members, int userCount, GET_MEMBERS_ERR errorCode)
      {
          (IChannelMember **) members;
          (int) userCount;
      }
        
      /**
       Occurs when channel attributes are updated, and returns all attributes of the channel. 
       
       @note This callback is enabled only when the user, who updates the attributes of the channel, sets \ref ar::rtm::ChannelAttributeOptions::enableNotificationToChannelMembers "enableNotificationToChannelMembers" as true. Also note that this flag is valid only within the current channel attribute method call.

       @param attributes All attribute of this channel.
       @param numberOfAttributes The total number of the channel attributes.
       */
      virtual void onAttributesUpdated(const IRtmChannelAttribute* attributes[], int numberOfAttributes)
      {
          (const IRtmChannelAttribute**) attributes;
          (int) numberOfAttributes;
      }
       
      /**
       Occurs when the number of the channel members changes, and returns the new number.

       @note 
       - When the number of channel members &le; 512, the SDK returns this callback when the number changes and at a MAXIMUM speed of once per second.
       - When the number of channel members exceeds 512, the SDK returns this callback when the number changes and at a MAXIMUM speed of once every three seconds.
       - You will receive this callback when successfully joining an RTM channel, so Agore recommends implementing this callback to receive timely updates on the number of the channel members.

       @param memberCount Member count of this channel.
       */
      virtual void onMemberCountUpdated(int memberCount)
      {
         (int) memberCount;
      }
    };

    /**
     @brief AR RTM channel methods.
     */
    class IChannel
    {
    protected:
      virtual ~IChannel() {}
    public:
//            virtual void setEventHandler(IChannelEventHandler *eventHandler) = 0;

      /**
       Joins a channel.
       
       @note You can join a maximum of 20 RTM channels at the same time. When the number of the channels you join exceeds the limit, you receive the \ref ar::rtm::JOIN_CHANNEL_ERR "JOIN_CHANNEL_ERR_FAILURE" error code.

       - If this method call succeeds:
          - The local user receives the \ref ar::rtm::IChannelEventHandler::onJoinSuccess "onJoinSuccess" callback.
          - All remote users receive the \ref ar::rtm::IChannelEventHandler::onMemberJoined "onMemberJoined" callback.
       - If this method call fails, the local user receives the \ref ar::rtm::IChannelEventHandler::onJoinFailure "onJoinFailure" callback. See #JOIN_CHANNEL_ERR for the error codes.
       @return
       - 0: Success.
       - &ne;0: Failure. See #JOIN_CHANNEL_ERR for the error codes.
      */
      virtual int join() = 0;
 
      /**
       Leaves a channel.

       - If this method call succeeds:
          - The local user receives the \ref ar::rtm::IChannelEventHandler::onLeave "onLeave" callback with the LEAVE_CHANNEL_ERR_OK state.
          - All remote users receive the \ref ar::rtm::IChannelEventHandler::onMemberLeft "onMemberLeft" callback.
       - If this method call fails, the local user receives the \ref ar::rtm::IChannelEventHandler::onLeave "onLeave" callback with an error code. See \ref ar::rtm::LEAVE_CHANNEL_ERR "LEAVE_CHANNEL_ERR" for the error codes.
       @return
       - 0: Success.
       - &ne;0: Failure. See #LEAVE_CHANNEL_ERR for the error codes.
       */
      virtual int leave() = 0;
 
      /**
       @deprecated Allows a channel member to send a message to all members in the channel.
       
       AR does not recommend using this method to send a channel message. Use \ref ar::rtm::IChannel::sendMessage(const IMessage *message, const SendMessageOptions &options) "sendMessage" instead.

       If this method call succeeds:
       - The \ref ar::rtm::IChannelEventHandler::onSendMessageResult "onSendMessageResult" callback returns the result.
       - All remote users in the channel receive the \ref ar::rtm::IChannelEventHandler::onMessageReceived "onMessageReceived" callback.
       
       @note You can send messages, including peer-to-peer and channel messages, at a maximum speed of 60 queries per second.

       @param message The message to be sent. See \ref ar::rtm::IMessage "IMessage".
       @return
       - 0: Success.
       - &ne;0: Failure. See #CHANNEL_MESSAGE_ERR_CODE for the error codes.
       */
      virtual int sendMessage(const IMessage *message) = 0;

      /**
       Allows a channel member to send a message to all members in the channel.
       
       If this method call succeeds:
       - The \ref ar::rtm::IChannelEventHandler::onSendMessageResult "onSendMessageResult" callback returns the result.
       - All remote users in the channel receive the \ref ar::rtm::IChannelEventHandler::onMessageReceived "onMessageReceived" callback.
       
       @note You can send messages, including peer-to-peer and channel messages, at a maximum speed of 60 queries per second.

       @param message The message to be sent. See \ref ar::rtm::IMessage "IMessage".
       @param options Options when sending the channel message. See See \ref ar::rtm::SendMessageOptions "SendMessageOptions".
       @return
       - 0: Success.
       - &ne;0: Failure. See #CHANNEL_MESSAGE_ERR_CODE for the error codes.
       */
      virtual int sendMessage(const IMessage *message, const SendMessageOptions &options) = 0;
        
      /**
       Retrieves the channel ID.

       @return The channel ID of the channel.
       */
      virtual const char *getId() const = 0;
        
      /**
       Retrieves a member list of the channel.

       The \ref ar::rtm::IChannelEventHandler::onGetMembers "onGetMembers" callback returns the result of this method call.
       
       @note You can call this method at a maximum speed of five queries every two seconds.

       @return
       - 0: Success.
       - &ne;0: Failure. See #GET_MEMBERS_ERR for the error codes.
       */
      virtual int getMembers() = 0;

      // sync_call
        
      /**
       Releases all resources used by the \ref ar::rtm::IChannel "IChannel" instance.
       */
      virtual void release() = 0;
    };

    class IRtmServiceEventHandler
    {
    public:
      virtual ~IRtmServiceEventHandler()
      {
      }

      /**
       Occurs when a user logs in the AR RTM system.

       The local user receives this callback when the \ref ar::rtm::IRtmService::login "login" method call succeeds.
       */
      virtual void onLoginSuccess() {}
        
      /**
       Occurs when a user fails to log in the AR RTM system.

       The local user receives this callback when the \ref ar::rtm::IRtmService::login "login" method call fails. See \ref ar::rtm::LOGIN_ERR_CODE "LOGIN_ERR_CODE" for the error codes.
       */
      virtual void onLoginFailure(LOGIN_ERR_CODE errorCode)
      {
          (LOGIN_ERR_CODE) errorCode;
      }

      /**
       Reports the result of the \ref ar::rtm::IRtmService::renewToken "renewToken" method call.
       
       @param token Your new token.
       @param errorCode The error code. See #RENEW_TOKEN_ERR_CODE.
       */
      virtual void onRenewTokenResult(const char* token, RENEW_TOKEN_ERR_CODE errorCode)
      {
          (const char*) token;
          (RENEW_TOKEN_ERR_CODE) errorCode;
      }
 
      /**
       Occurs when the RTM server detects that the RTM token has exceeded the 24-hour validity period and when the SDK is in the \ref ar::rtm::CONNECTION_STATE_RECONNECTING "CONNECTION_STATE_RECONNECTING" state.
       
       - This callback occurs only when the SDK is reconnecting to the server. You will not receive this callback when the SDK is in the \ref ar::rtm::CONNECTION_STATE_CONNECTED "CONNECTION_STATE_CONNECTED" state. 
       - When receiving this callback, generate a new RTM Token on the server and call the \ref ar::rtm::IRtmService::renewToken "renewToken" method to pass the new Token on to the server.
       */
      virtual void onTokenExpired()
      {
      }

      /**
       Occurs when a user logs out of the AR RTM system.

       The local user receives this callback when the SDK calls the \ref ar::rtm::IRtmService::logout "logout" method. See \ref ar::rtm::LOGOUT_ERR_CODE "LOGOUT_ERR_CODE" for the error codes.
       */
      virtual void onLogout(LOGOUT_ERR_CODE errorCode)
      {
          (LOGOUT_ERR_CODE) errorCode;
      }
        
      /**
       Occurs when the connection state changes between the SDK and the AR RTM system.

       @param state The new connection state. See #CONNECTION_STATE.
       @param reason The reason for the connection state change. See #CONNECTION_CHANGE_REASON.
       */
      virtual void onConnectionStateChanged(CONNECTION_STATE state, CONNECTION_CHANGE_REASON reason)
      {
      }
        
      /**
       Reports the result of the \ref ar::rtm::IRtmService::sendMessageToPeer "sendMessageToPeer" method call.

       @param messageId The ID of the sent message.
       @param errorCode The peer-to-peer message state. See #PEER_MESSAGE_ERR_CODE.

       */
      virtual void onSendMessageResult(long long messageId, PEER_MESSAGE_ERR_CODE errorCode)
      {
          (long long) messageId;
          (PEER_MESSAGE_ERR_CODE) errorCode;
      }
    
      /**
       Occurs when receiving a peer-to-peer message.

       @param peerId The ID of the message sender.
       @param message The received peer-to-peer message. See \ref ar::rtm::IMessage "IMessage".
       */
      virtual void onMessageReceivedFromPeer(const char *peerId, const IMessage *message)
      {
          (char *) peerId;
          (IMessage *) message;
      }
        
      /**
       Occurs when receiving a peer-to-peer image message.
       
       @param peerId The ID of the message sender.
       @param message The received peer-to-peer image message. See \ref ar::rtm::IImageMessage "IImageMessage".
       */
      virtual void onImageMessageReceivedFromPeer(const char *peerId, const IImageMessage* message)
      {
          (char *) peerId;
          (IImageMessage *) message;
      }
        
      /**
       Occurs when receiving a peer-to-peer file message.
     
       @param peerId The ID of the message sender.
       @param message The received peer-to-peer file message. See \ref ar::rtm::IFileMessage "IFileMessage".
       */
      virtual void onFileMessageReceivedFromPeer(const char *peerId, const IFileMessage* message)
      {
          (char *) peerId;
          (IFileMessage *) message;
      }
        
      /**
       Reports the progress of an ongoing upload task.
       
       @note
       - If the upload task is ongoing, the SDK returns this callback once every second.
       - If the upload task comes to a halt, the SDK stops returning this callback until the task is going again.

       @param requestId The unique ID of the upload request.
       @param progress The progress of the ongoing upload task. See \ref ar::rtm::MediaOperationProgress "MediaOperationProgress".
       */
      virtual void onMediaUploadingProgress(long long requestId, const MediaOperationProgress &progress)
      {
          (long long) requestId;
          (MediaOperationProgress) progress;
      }
        
      /**
       Reports the progress of an ongoing download task.
       
       @note
       - If the download task is ongoing, the SDK returns this callback once every second.
       - If the download task comes to a halt, the SDK stops returning this callback until the task is going again.
     
       @param requestId The unique ID of the download request.
       @param progress The progress of the ongoing download task. See \ref ar::rtm::MediaOperationProgress "MediaOperationProgress".
       */
      virtual void onMediaDownloadingProgress(long long requestId, const MediaOperationProgress &progress)
      {
          (long long) requestId;
          (MediaOperationProgress) progress;
      }

      /**
       Reports the result of the \ref ar::rtm::IRtmService::createFileMessageByUploading "createFileMessageByUploading" method call.
     
       @param requestId The unique ID of the upload request.
       @param fileMessage An \ref ar::rtm::IFileMessage "IFileMessage" instance.
       @param code Error codes. See #UPLOAD_MEDIA_ERR_CODE.
       */
      virtual void onFileMediaUploadResult(long long requestId, IFileMessage* fileMessage, UPLOAD_MEDIA_ERR_CODE code)
      {
          (long long) requestId;
          (IFileMessage *) fileMessage;
          (UPLOAD_MEDIA_ERR_CODE) code;
      }
   
      /**
       Reports the result of the \ref ar::rtm::IRtmService::createImageMessageByUploading "createImageMessageByUploading" method call.

       @param requestId The unique ID of the upload request.
       @param imageMessage An \ref ar::rtm::IImageMessage "IImageMessage" instance.
       @param code Error codes. See #UPLOAD_MEDIA_ERR_CODE.
       */
      virtual void onImageMediaUploadResult(long long requestId, IImageMessage* imageMessage, UPLOAD_MEDIA_ERR_CODE code)
      {
          (long long) requestId;
          (IImageMessage *) imageMessage;
          (UPLOAD_MEDIA_ERR_CODE) code;
      }

      /**
       Reports the result of the \ref ar::rtm::IRtmService::downloadMediaToFile "downloadMediaToFile" method call.
     
       @param requestId The unique ID of the download request.
       @param code Error codes. See #DOWNLOAD_MEDIA_ERR_CODE.
       */
      virtual void onMediaDownloadToFileResult(long long requestId, DOWNLOAD_MEDIA_ERR_CODE code)
      {
          (long long)requestId;
          (DOWNLOAD_MEDIA_ERR_CODE)code;
      }
        
      /**
       Reports the result of the \ref ar::rtm::IRtmService::downloadMediaToMemory "downloadMediaToMemory" method call.
       
       @note The SDK releases the downloaded file or image immediately after returning this callback.
     
       @param requestId The unique ID of the download request.
       @param memory The memory address where the downloaded file or image is stored.
       @param length The size of the downloaded file or image.
       @param code Error codes. See #DOWNLOAD_MEDIA_ERR_CODE.
       */
      virtual void onMediaDownloadToMemoryResult(long long requestId, const char* memory, long long length, DOWNLOAD_MEDIA_ERR_CODE code)
      {
          (long long) requestId;
          (const char*) memory;
          (long long) length;
          (DOWNLOAD_MEDIA_ERR_CODE) code;
      }
        
      /**
       Reports the result of the \ref ar::rtm::IRtmService::cancelMediaDownload "cancelMediaDownload" or \ref ar::rtm::IRtmService::cancelMediaUpload "cancelMediaUpload" method call.
     
       @param requestId The unique ID of the cancel request.
       @param code Error codes. See #CANCEL_MEDIA_ERR_CODE.
       */
      virtual void onMediaCancelResult(long long requestId, CANCEL_MEDIA_ERR_CODE code)
      {
          (long long) requestId;
          (CANCEL_MEDIA_ERR_CODE) code;
      }

      /**
       Reports the result of the \ref ar::rtm::IRtmService::queryPeersOnlineStatus "queryPeersOnlineStatus" method call.

       @param requestId The unique ID of this request.
       @param peersStatus The online status of the peer. See PeerOnlineStatus.
       @param peerCount The number of the queried peers.
       @param errorCode Error Codes. See #QUERY_PEERS_ONLINE_STATUS_ERR.
       */
      virtual void onQueryPeersOnlineStatusResult(long long requestId, const PeerOnlineStatus* peersStatus, int peerCount, QUERY_PEERS_ONLINE_STATUS_ERR errorCode)
      {
          (long long) requestId; 
          (const PeerOnlineStatus*) peersStatus;
          (int) peerCount;
          (QUERY_PEERS_ONLINE_STATUS_ERR) errorCode;
      }

      /**
       Returns the result of the \ref ar::rtm::IRtmService::subscribePeersOnlineStatus "subscribePeersOnlineStatus" or \ref ar::rtm::IRtmService::unsubscribePeersOnlineStatus "unsubscribePeersOnlineStatus" method call.

       @param requestId The unique ID of this request.
       @param errorCode Error Codes. See #PEER_SUBSCRIPTION_STATUS_ERR.
       */
      virtual void onSubscriptionRequestResult(long long requestId, PEER_SUBSCRIPTION_STATUS_ERR errorCode)
      {
          (long long)requestId;
          (PEER_SUBSCRIPTION_STATUS_ERR)errorCode;
      }

      /**
       Returns the result of the \ref ar::rtm::IRtmService::queryPeersBySubscriptionOption "queryPeersBySubscriptionOption" method call.

       @param requestId The unique ID of this request.
       @param peerIds A user ID array of the specified users, to whom you subscribe.
       @param peerCount Count of the peer(s).
       @param errorCode Error Codes. See #QUERY_PEERS_BY_SUBSCRIPTION_OPTION_ERR.
       */
      virtual void onQueryPeersBySubscriptionOptionResult(long long requestId, const char* peerIds[], int peerCount, QUERY_PEERS_BY_SUBSCRIPTION_OPTION_ERR errorCode)
      {
          (const char**)peerIds;
          (int)peerCount;
          (QUERY_PEERS_BY_SUBSCRIPTION_OPTION_ERR)errorCode;
      }

      /**
       Occurs when the online status of the peers, to whom you subscribe, changes.
       
       - When the subscription to the online status of specified peer(s) succeeds, the SDK returns this callback to report the online status of peers, to whom you subscribe.
       - When the online status of the peers, to whom you subscribe, changes, the SDK returns this callback to report whose online status has changed.
       - If the online status of the peers, to whom you subscribe, changes when the SDK is reconnecting to the server, the SDK returns this callback to report whose online status has changed when successfully reconnecting to the server.
       
       @param peersStatus An array of peers' online states. See PeerOnlineStatus.
       @param peerCount Count of the peer(s), whose online status changes.
       */
      virtual void onPeersOnlineStatusChanged(const PeerOnlineStatus peersStatus[], int peerCount)
      {
          (const PeerOnlineStatus*)peersStatus;
          (int)peerCount;
      }

      /**
       Reports the result of the \ref ar::rtm::IRtmService::setLocalUserAttributes "setLocalUserAttributes" method call.

       @param requestId The unique ID of this request.
       @param errorCode Error Codes. See #ATTRIBUTE_OPERATION_ERR.
       */
      virtual void onSetLocalUserAttributesResult(long long requestId, ATTRIBUTE_OPERATION_ERR errorCode)
      {
          (long long) requestId;
          (ATTRIBUTE_OPERATION_ERR) errorCode;
      }

      /**
       Reports the result of the \ref ar::rtm::IRtmService::addOrUpdateLocalUserAttributes "addOrUpdateLocalUserAttributes" method call.

       @param requestId The unique ID of this request.
       @param errorCode Error Codes. See #ATTRIBUTE_OPERATION_ERR.
       */
      virtual void onAddOrUpdateLocalUserAttributesResult(long long requestId, ATTRIBUTE_OPERATION_ERR errorCode)
      {
         (long long) requestId;
         (ATTRIBUTE_OPERATION_ERR) errorCode;
      }

      /**
       Reports the result of the \ref ar::rtm::IRtmService::deleteLocalUserAttributesByKeys "deleteLocalUserAttributesByKeys" method call.

       @param requestId The unique ID of this request.
       @param errorCode Error Codes. See #ATTRIBUTE_OPERATION_ERR.
       */
      virtual void onDeleteLocalUserAttributesResult(long long requestId, ATTRIBUTE_OPERATION_ERR errorCode)
      {
         (long long) requestId;
         (ATTRIBUTE_OPERATION_ERR) errorCode;
      }

      /**
       Reports the result of the \ref ar::rtm::IRtmService::clearLocalUserAttributes "clearLocalUserAttributes" method call.

       @param requestId The unique ID of this request.
       @param errorCode Error Codes. See #ATTRIBUTE_OPERATION_ERR.
       */
      virtual void onClearLocalUserAttributesResult(long long requestId, ATTRIBUTE_OPERATION_ERR errorCode)
      {
          (long long) requestId;
          (ATTRIBUTE_OPERATION_ERR) errorCode;
      }

      /**
       Reports the result of the \ref ar::rtm::IRtmService::getUserAttributes "getUserAttributes" or \ref ar::rtm::IRtmService::getUserAttributesByKeys "getUserAttributesByKeys" method call.

       @param requestId The unique ID of this request.
       @param userId The user ID of the specified user.
       @param attributes An array of the returned attributes. See RtmAttribute.
       @param numberOfAttributes The total number of the user's attributes
       @param errorCode Error Codes. See #ATTRIBUTE_OPERATION_ERR.
       */
      virtual void onGetUserAttributesResult(long long requestId, const char* userId, const RtmAttribute* attributes, int numberOfAttributes, ATTRIBUTE_OPERATION_ERR errorCode)
      {
          (long long) requestId;
          (const RtmAttribute*) attributes;
          (int) numberOfAttributes;
          (ATTRIBUTE_OPERATION_ERR) errorCode;
      }
      
      /**
       Reports the result of the \ref ar::rtm::IRtmService::setChannelAttributes "setChannelAttributes" method call.

       @param requestId The unique ID of this request.
       @param errorCode Error Codes. See #ATTRIBUTE_OPERATION_ERR.
       */
      virtual void onSetChannelAttributesResult(long long requestId, ATTRIBUTE_OPERATION_ERR errorCode)
      {
         (long long) requestId;
         (ATTRIBUTE_OPERATION_ERR) errorCode;
      }
      
      /**
       Reports the result of the \ref ar::rtm::IRtmService::addOrUpdateChannelAttributes "addOrUpdateChannelAttributes" method call.

       @param requestId The unique ID of this request.
       @param errorCode Error Codes. See #ATTRIBUTE_OPERATION_ERR.
       */
      virtual void onAddOrUpdateChannelAttributesResult(long long requestId, ATTRIBUTE_OPERATION_ERR errorCode)
      {
         (long long) requestId;
         (ATTRIBUTE_OPERATION_ERR) errorCode;
      }
      
      /**
       Reports the result of the \ref ar::rtm::IRtmService::deleteChannelAttributesByKeys "deleteChannelAttributesByKeys" method call.

       @param requestId The unique ID of this request.
       @param errorCode Error Codes. See #ATTRIBUTE_OPERATION_ERR.
       */
      virtual void onDeleteChannelAttributesResult(long long requestId, ATTRIBUTE_OPERATION_ERR errorCode)
      {
         (long long) requestId;
         (ATTRIBUTE_OPERATION_ERR) errorCode;
      }
      
      /**
       Reports the result of the \ref ar::rtm::IRtmService::clearChannelAttributes "clearChannelAttributes" method call.

       @param requestId The unique ID of this request.
       @param errorCode Error Codes. See #ATTRIBUTE_OPERATION_ERR.
       */
      virtual void onClearChannelAttributesResult(long long requestId, ATTRIBUTE_OPERATION_ERR errorCode)
      {
         (long long) requestId;
         (ATTRIBUTE_OPERATION_ERR) errorCode;
      }
      
      /**
       Reports the result of the \ref ar::rtm::IRtmService::getChannelAttributes "getChannelAttributes" or \ref ar::rtm::IRtmService::getChannelAttributesByKeys "getChannelAttributesByKeys" method call.

       @param requestId The unique ID of this request.
       @param attributes An array of the returned channel attributes. 
       @param numberOfAttributes The total number of the attributes.
       @param errorCode Error Codes. See #ATTRIBUTE_OPERATION_ERR.
       */
      virtual void onGetChannelAttributesResult(long long requestId, const IRtmChannelAttribute* attributes[], int numberOfAttributes, ATTRIBUTE_OPERATION_ERR errorCode)
      {
//         (long long) requestId;
//         (const IRtmChannelAttribute**) attributes;
//         (int) numberOfAttributes;
//         (ATTRIBUTE_OPERATION_ERR) errorCode;
      }
       
      /**
       Reports the result of the \ref ar::rtm::IRtmService::getChannelMemberCount "getChannelMemberCount" method call.

       @param requestId The unique ID of this request.
       @param channelMemberCounts An array of the channel member counts.
       @param channelCount The total number of the channels. 
       @param errorCode Error Codes. See #GET_CHANNEL_MEMBER_COUNT_ERR_CODE.
       */
      virtual void onGetChannelMemberCountResult(long long requestId, const ChannelMemberCount* channelMemberCounts , int channelCount, GET_CHANNEL_MEMBER_COUNT_ERR_CODE errorCode)
       {
          (long long) requestId;
          (const ChannelMemberCount*) channelMemberCounts;
          (int) channelCount;
          (GET_CHANNEL_MEMBER_COUNT_ERR_CODE)errorCode;
       }
    };

    class IRtmCallManager;
    class IRtmCallEventHandler;
      
    /**
     @brief Output log filter level.
     */
    enum LOG_FILTER_TYPE
    {
		
      /**
       0: Do not output any log information.
	     */
      LOG_FILTER_OFF = 0,
	  
      /**
       0x000f: Output CRITICAL, ERROR, WARNING, and INFO level log information.
	     */
      LOG_FILTER_INFO = 0x000f,
	  
      /**
       0x000e: Output CRITICAL, ERROR, and WARNING level log information.
	     */
      LOG_FILTER_WARN = 0x000e,
	  
      /**
       0x000c: Output CRITICAL and ERROR level log information.
	     */
      LOG_FILTER_ERROR = 0x000c,
	  
      /**
       0x0008: Output CRITICAL level log information.
	     */
      LOG_FILTER_CRITICAL = 0x0008,
      /**
       Reserved for future use. 
	     */	  
      LOG_FILTER_MASK = 0x80f,
    };

    class IRtmService
    {
    protected:
      virtual ~IRtmService() {}
    public:
        
      /**
       Initializes an IRtmService instance.

       All methods in the IRtmService class are executed asynchronously.

       @param appId The App ID issued by AR to you. Apply for a new App ID from AR if it is missing from your kit.
       @param eventHandler An IRtmServiceEventHandler object that invokes callbacks to be passed to the application on AR RTM SDK runtime events.
       @return
       - 0: Success.
       - < 0: Failure. 
       */
      virtual int initialize(const char *appId, IRtmServiceEventHandler *eventHandler) = 0;
        
      /**
       Adds an IRtmServiceEventHandler object.

       @param eventHandler An IRtmServiceEventHandler object.
       */
      virtual void addEventHandler(IRtmServiceEventHandler *eventHandler) = 0;
        
      /**
       Removes an IRtmServiceEventHandler object.

       @param eventHandler An IRtmServiceEventHandler object.
       */
      virtual void removeEventHandler(IRtmServiceEventHandler *eventHandler) = 0;

      /**
       Releases all resources used by the \ref ar::rtm::IRtmService "IRtmService" instance.
       */
      virtual void release(bool sync = false) = 0;

      /**
       Logs in the AR RTM system.

       - If this method call succeeds, the local user receives the \ref ar::rtm::IRtmServiceEventHandler::onLoginSuccess "onLoginSuccess" callback.
       - If this method call fails, the local user receives the \ref ar::rtm::IRtmServiceEventHandler::onLoginFailure "onLoginFailure" callback. See \ref ar::rtm::LOGIN_ERR_CODE "LOGIN_ERR_CODE" for the error codes.
              
       @note
       - If you log in with the same user ID from a different instance, you will be kicked out of your previous login and removed from previously joined channels.
       - The call frequency limit for this method is 2 queries per second.
       - Only after you successfully call this method (when the local user receives the \ref ar::rtm::IRtmServiceEventHandler::onLoginSuccess "onLoginSuccess" callback), can you call the key RTM methods except:
            - \ref ar::rtm::IRtmService::createChannel "createChannel"
            - \ref ar::rtm::IRtmService::createMessage "createMessage"
            - \ref ar::rtm::IMessage::setText "setText"
            - \ref ar::rtm::IRtmService::getRtmCallManager "getRtmCallManager"
            - \ref ar::rtm::IRtmCallManager::createLocalCallInvitation "createLocalCallInvitation"
            - \ref ar::rtm::IRtmService::createChannelAttribute "createChannelAttribute"

       @param token Token used to log in the AR RTM system and used when dynamic authentication is enabled. Set @p token as @p nil in the integration and test stages.
       @param userId The user ID of the user logging in AR's RTM system. The string must not exceed 64 bytes in length. It cannot be empty, null or "null".
       Supported characters:
       - All lowercase English letters: a to z
       - All uppercase English letters: A to Z
       - All numeric characters: 0 to 9
       - The space character.
       - Punctuation characters and other symbols, including: "!", "#", "$", "%", "&", "(", ")", "+", "-", ":", ";", "<", "=", ".", ">", "?", "@", "[", "]", "^", "_", " {", "}", "|", "~", ","
       @return
       - 0: Success.
       - &ne;0: Failure. See #LOGIN_ERR_CODE for the error codes.
       */
      virtual int login(const char *token, const char *userId) = 0;

      /**
       Logs out of the AR RTM system.

       The local user receives the \ref ar::rtm::IRtmServiceEventHandler::onLogout "onLogout" callback. See \ref ar::rtm::LOGOUT_ERR_CODE "LOGIN_ERR_CODE" for the error codes.
       
       @return
       - 0: Success.
       - &ne;0: Failure. See #LOGOUT_ERR_CODE for the error codes.
       */
      virtual int logout() = 0;
  
      /**
       Renews the current RTM Token.
       
       You are required to renew your RTM Token when receiving the \ref ar::rtm::IRtmServiceEventHandler::onTokenExpired "onTokenExpired" callback, and the \ref ar::rtm::IRtmServiceEventHandler::onRenewTokenResult "onRenewTokenResult" callback returns the result of this method call. The call frequency limit for this method is 2 queries per second.
       
       @param token Your new RTM Token.
       @return
       - 0: Success.
       - &ne;0: Failure. See #RENEW_TOKEN_ERR_CODE for the error codes.
       */
      virtual int renewToken(const char* token) = 0;

      /**
       @deprecated Sends a peer-to-peer message to a specified user (receiver).
       
       @note 
       - We do not recommend using this method to send a peer-to-peer message. Use \ref ar::rtm::IRtmService::sendMessageToPeer(const char *peerId, const IMessage *message, const SendMessageOptions &options) "sendMessageToPeer" instead.
       - You can send messages, including peer-to-peer and channel messages, at a maximum speed of 60 queries per second.

       @param peerId User ID of the receiver.
       @param message The message to be sent. For information about creating a message, see \ref ar::rtm::IMessage "IMessage".
       @return
       - 0: Success.
       - &ne;0: Failure. See #PEER_MESSAGE_ERR_CODE for the error codes.
       */
      virtual int sendMessageToPeer(const char *peerId, const IMessage *message) = 0;
      
      /**
       Downloads a file or image from the AR server to the local memory by media ID.

       The SDK returns the result of this method call by the \ref ar::rtm::IRtmServiceEventHandler::onMediaDownloadToFileResult "onMediaDownloadToMemoryResult" callback.
       
       @note 
       - This method applies to scenarios requiring quick access to the downloaded file or image.
       - The SDK releases the downloaded file or image immediately after returning the \ref ar::rtm::IRtmServiceEventHandler::onMediaDownloadToFileResult "onMediaDownloadToMemoryResult" callback.

       @param mediaId The media ID of the file or image on the AR server.
       @param requestId The unique ID of this download request.
       @return
       - 0: Success.
       - &ne;0: Failure. See #DOWNLOAD_MEDIA_ERR_CODE for the error codes.
       */
      virtual int downloadMediaToMemory(const char* mediaId, long long &requestId) = 0;
      
      /**
       Downloads a file or image from the AR server to a specified local directory by media ID.

       The SDK returns the result of this method call by the \ref ar::rtm::IRtmServiceEventHandler::onMediaDownloadToFileResult "onMediaDownloadToFileResult" callback.

       @param mediaId The media ID of the file or image on the AR server.
       @param filePath The full path to the downloaded file or image. Must be in UTF-8.
       @param requestId The unique ID of this download request.
       @return
       - 0: Success.
       - &ne;0: Failure. See #DOWNLOAD_MEDIA_ERR_CODE for the error codes.
       */
      virtual int downloadMediaToFile(const char* mediaId, const char* filePath, long long &requestId) = 0;
      
      /**
       Cancels an ongoing file or image download task by request ID.

       The SDK returns the result of this method call with the \ref ar::rtm::IRtmServiceEventHandler::onMediaCancelResult "onMediaCancelResult" callback.
       
       @note
       You can only cancel an ongoing download task. After a download task completes, the corresponding request ID is no longer valid and hence you cannot cancel it.

       @param requestId The unique ID of the download request to cancel.
       @return
       - 0: Success.
       - &ne;0: Failure. See #CANCEL_MEDIA_ERR_CODE for the error codes.
       */
      virtual int cancelMediaDownload(long long requestId) = 0;
      
      /**
       Cancels an ongoing file or image upload task by request ID.

       The SDK returns the result of this method call with the \ref ar::rtm::IRtmServiceEventHandler::onMediaCancelResult "onMediaCancelResult" callback.
       
       @note
       You can only cancel an ongoing upload task. After an upload task completes, you cannot cancel it and the corresponding request ID is no longer valid.

       @param requestId The unique ID of the upload request to cancel.
       @return
       - 0: Success.
       - &ne;0: Failure. See #CANCEL_MEDIA_ERR_CODE for the error codes.
       */
      virtual int cancelMediaUpload(long long requestId) = 0;
      
        
      /**
       Sends an (offline) peer-to-peer message to a specified user (receiver).
  
       This method allows you to send a message to a specified user when he/she is offline. If you set a message as an offline message and the specified user is offline when you send it, the RTM server caches it. Please note that for now we only cache 200 offline messages for up to seven days for each receiver. When the number of the cached messages reaches this limit, the newest message overrides the oldest one.
       
       If you use this method to send off a <i>text</i> message that starts off with AGORA_RTM_ENDCALL_PREFIX_\<channelId\>_\<your additional information\>, then this method is compatible with the endCall method of the legacy AR Signaling SDK. Replace \<channelId\> with the channel ID from which you want to leave (end call), and replace \<your additional information\> with any additional information. Note that you must not put any "_" (underscore" in your additional information but you can set \<your additional information\> as empty "".
       
       - The \ref ar::rtm::IRtmServiceEventHandler::onSendMessageResult "onSendMessageResult" callback returns the result of this method call.
       - When the message arrives at the receiver, the receiver receives the \ref ar::rtm::IRtmServiceEventHandler::onMessageReceivedFromPeer "onMessageReceivedFromPeer" callback.
       
       If you use this method to send off a <i>text</i> message that starts off with AGORA_RTM_ENDCALL_PREFIX_\<channelId\>_\<your additional information\>, then this method is compatible with the endCall method of the legacy AR Signaling SDK. Replace \<channelId\> with the channel ID from which you want to leave (end call), and replace \<your additional information\> with any additional information. Note that you must not put any "_" (underscore" in your additional information but you can set \<your additional information\> as empty "".

       @note You can send messages, including peer-to-peer and channel messages, at a maximum speed of 60 queries per second.

       @param peerId User ID of the receiver. The string must not exceed 64 bytes in length. It cannot be empty, null, or "null". Supported characters:
       - The 26 lowercase English letters: a to z
       - The 26 uppercase English letters: A to Z
       - The 10 numbers: 0 to 9
       - Space
       - "!", "#", "$", "%", "&", "(", ")", "+", "-", ":", ";", "<", "=", ".", ">", "?", "@", "[", "]", "^", "_", " {", "}", "|", "~", ","
       @param message The message to be sent. For information about creating a message, See \ref ar::rtm::IMessage "IMessage".
       @param options Options when sending the message to a peer. See \ref ar::rtm::SendMessageOptions "SendMessageOptions".
       @return
       - 0: Success.
       - &ne;0: Failure. See #PEER_MESSAGE_ERR_CODE for the error codes.
       */
      virtual int sendMessageToPeer(const char *peerId, const IMessage *message, const SendMessageOptions &options) = 0;

      /**
       Creates an AR RTM channel.

       - If the method call succeeds, the SDK returns an IChannel instance.
       - If this method call fails, the SDK returns @p null.

       @note You can create multiple channels in an IRtmService instance. But you can only join a maximum of 20 channels at the same time. As a good practice, Agore recommends calling the \ref ar::rtm::IChannel::release "release" method to release all resources of an RTM channel that you no longer use.

       @param channelId The unique channel name. A @p channelId cannot be empty, null, or "null". Must be less than 64 bytes in length.
       Supported characters:
       - All lowercase English letters: a to z
       - All uppercase English letters: A to Z
       - All numeric characters: 0 to 9
       - The space character.
       - Punctuation characters and other symbols, including: "!", "#", "$", "%", "&", "(", ")", "+", "-", ":", ";", "<", "=", ".", ">", "?", "@", "[", "]", "^", "_", " {", "}", "|", "~", ","
       @param eventHandler See IChannelEventHandler.
       @return
       - An \ref ar::rtm::IChannel "IChannel" object: Success.
       - null: Failure. Possible reasons: The @p channelId is invalid, or a channel with the same @p channelId already exists.
       */
      virtual IChannel * createChannel(const char *channelId, IChannelEventHandler *eventHandler) = 0;
        
      /**
       Gets an \ref ar::rtm::IRtmCallManager "IRtmCallManager" object.
     
       Each \ref ar::rtm::IRtmService "IRtmService" object corresponds to a unique \ref ar::rtm::IRtmCallManager "IRtmCallManager" object. That said, the \ref ar::rtm::IRtmCallManager "IRtmCallManager" object is \ref ar::rtm::IRtmService "IRtmService"-specific.
     
       @note When you do no use \ref ar::rtm::IRtmCallManager "IRtmCallManager", call the \ref ar::rtm::IRtmCallManager::release "release" method to release all of its resources.
     
       @param eventHandler An \ref ar::rtm::IRtmCallEventHandler "IRtmCallEventHandler" object.
       @return An \ref ar::rtm::IRtmCallManager "IRtmCallManager" object.
       */
      virtual IRtmCallManager* getRtmCallManager(IRtmCallEventHandler*eventHandler) = 0;
        
      /**
       Creates an empty text \ref ar::rtm::IMessage "IMessage" instance.
       
       @note
       - An \ref ar::rtm::IMessage "IMessage" instance can be used either for a channel message or for a peer-to-peer message.
       - When you no longer need an \ref ar::rtm::IMessage "IMessage" instance, please call the \ref ar::rtm::IMessage::release "release" method to release all resources that it is using.
       - You can set the content of the text message at a later point using the \ref ar::rtm::IMessage::setText "setText" method, but please ensure that the size of the text message does not exceed 32 KB.
       
       @return An empty text \ref ar::rtm::IMessage "IMessage" instance.
       */
      virtual IMessage *createMessage() = 0;
    
      /**
       Creates a text \ref ar::rtm::IMessage "IMessage" instance.
     
       @note
       - An \ref ar::rtm::IMessage "IMessage" instance can be used either for a channel message or for a peer-to-peer message.
       - When you no longer need an \ref ar::rtm::IMessage "IMessage" instance, please call the \ref ar::rtm::IMessage::release "release" method to release all resources that it is using.
       
       @param message The text content of the message. Must not exceed 32 KB in length.
     
       @return A text \ref ar::rtm::IMessage "IMessage" instance.
       */
      virtual IMessage *createMessage(const char* message) = 0;
        
      /**
       Creates a raw \ref ar::rtm::IMessage "IMessage" instance.
     
       @note
       - An \ref ar::rtm::IMessage "IMessage" instance can be used either for a channel message or for a peer-to-peer message.
       - When you no longer need an \ref ar::rtm::IMessage "IMessage" instance, please call the \ref ar::rtm::IMessage::release "release" method to release all resources that it is using.
       - You can set a brief text description of the raw message at a later point using the \ref ar::rtm::IMessage::setText "setText" method, but please ensure that the size of the raw message and the description combined does not exceed 32 KB.
     
       @param rawData The starting address of the raw message.
       @param length Length of the raw message. Must not exceed 32 KB in length.
     
       @return A raw \ref ar::rtm::IMessage "IMessage" instance.
       */
      virtual IMessage *createMessage(const uint8_t* rawData, int length) = 0;
        
      /**
       Creates a raw \ref ar::rtm::IMessage "IMessage" instance and sets its description.
     
       @note
       - An \ref ar::rtm::IMessage "IMessage" instance can be used either for a channel message or for a peer-to-peer message.
       - When you no longer need an \ref ar::rtm::IMessage "IMessage" instance, please call the \ref ar::rtm::IMessage::release "release" method to release all resources that it is using.
       - You can set @p description as "" and add a brief text description of the raw message at a later point using the \ref ar::rtm::IMessage::setText "setText" method, but please ensure that the size of the raw message and the description combined does not exceed 32 KB.
       
       @param rawData The starting address of the raw message.
       @param length The length of the raw message. Must not exceed 32 KB in length.
       @param description A brief text description of the raw message. If you set a text description, ensure that the size of the raw message and the description combined does not exceed 32 KB.
     
       @return A raw \ref ar::rtm::IMessage "IMessage" instance with a brief text description.
       */
      virtual IMessage *createMessage(const uint8_t* rawData, int length, const char* description) = 0;
        
      /**
       Creates an \ref ar::rtm::IFileMessage "IFileMessage" instance by media ID.
       
       - If you have at hand the media ID of a file on the AR server, you can call this method to create an \ref ar::rtm::IFileMessage "IFileMessage" instance.
       - If you do not have a media ID, then you must call \ref ar::rtm::IRtmService::createFileMessageByUploading "createFileMessageByUploading" to get a corresponding \ref ar::rtm::IFileMessage "IFileMessage" instance by uploading a file to the AR RTM server.
     
       @param mediaId The media ID of an uploaded file on the AR server.
       @return An \ref ar::rtm::IFileMessage "IFileMessage" instance.
       */
      virtual IFileMessage *createFileMessageByMediaId(const char* mediaId) = 0;
        
      /**
       Creates an \ref ar::rtm::IImageMessage "IImageMessage" instance by media ID.
       
       - If you have at hand the media ID of an image on the AR server, you can call this method to create an \ref ar::rtm::IImageMessage "IImageMessage" instance.
       - If you do not have a media ID, then you must call \ref ar::rtm::IRtmService::createImageMessageByUploading "createImageMessageByUploading" to get a corresponding \ref ar::rtm::IImageMessage "IImageMessage" instance by uploading an image to the AR RTM server.
       
       @param mediaId The media ID of an uploaded image on the AR server.
       @return An \ref ar::rtm::IImageMessage "IImageMessage" instance.
       */
      virtual IImageMessage *createImageMessageByMediaId(const char* mediaId) = 0;
        
      /**
       Gets an \ref ar::rtm::IFileMessage "IFileMessage" instance by uploading a file to the AR server.
       
       The SDK returns the result with the \ref ar::rtm::IRtmServiceEventHandler::onFileMediaUploadResult callback. If success, this callback returns a corresponding \ref ar::rtm::IFileMessage "IFileMessage" instance.
       
       @note
       - If you have at hand the media ID of a file on the AR server, you can call \ref ar::rtm::IRtmService::createFileMessageByMediaId "createFileMessageByMediaId" to create an \ref ar::rtm::IFileMessage "IFileMessage" instance.
       - To cancel an ongoing file upload task, call \ref ar::rtm::IRtmService::cancelMediaUpload "cancelMediaUpload".

     
       @param filePath The full path to the local file to upload. Must be in UTF-8.
       @param requestId The unique ID of this upload request.
       @return
       - 0: Success.
       - &ne;0: Failure. See #UPLOAD_MEDIA_ERR_CODE for the error codes.
       */
      virtual int createFileMessageByUploading(const char* filePath, long long &requestId) = 0;
        
      /**
       Gets an \ref ar::rtm::IImageMessage "IImageMessage" instance by uploading an image to the AR server.
     
       The SDK returns the result by the \ref ar::rtm::IRtmServiceEventHandler::onImageMediaUploadResult callback. If success, this callback returns a corresponding \ref ar::rtm::IImageMessage "ImageMessage" instance.
       
       - If the uploaded image is in JPEG, JPG, BMP, or PNG format, the SDK calculates the width and height of the image. You can call \ref ar::rtm::IImageMessage::getWidth "getWidth" and \ref ar::rtm::IImageMessage::getHeight "getHeight" to get the calculated width and height.
       - Otherwise, you need to call \ref ar::rtm::IImageMessage::setWidth "setWidth" and \ref ar::rtm::IImageMessage::setHeight "setHeight" to set the width and height of the uploaded image by yourself.

       @note
       - If you have at hand the media ID of an image on the AR server, you can call \ref ar::rtm::IRtmService::createImageMessageByMediaId "createImageMessageByMediaId" to create an \ref ar::rtm::IImageMessage "IImageMessage" instance.
       - To cancel an ongoing image upload task, call \ref ar::rtm::IRtmService::cancelMediaUpload "cancelMediaUpload".
     
       @param filePath The full path to the local image to upload. Must be in UTF-8.
       @param requestId The unique ID of the upload request.
       @return
       - 0: Success.
       - &ne;0: Failure. See #UPLOAD_MEDIA_ERR_CODE for the error codes.
       */
      virtual int createImageMessageByUploading(const char* filePath, long long &requestId) = 0;
        
      /**
       Creates an \ref ar::rtm::IRtmChannelAttribute "IRtmChannelAttribute" instance.
     
       @return An \ref ar::rtm::IRtmChannelAttribute "IRtmChannelAttribute" instance.
       */
      virtual IRtmChannelAttribute *createChannelAttribute() = 0;
        
      /**
       Provides the technical preview functionalities or special customizations by configuring the SDK with JSON options.
       
       @note The JSON options are not public by default. AR is working on making commonly used JSON options public in a standard way. Contact [support@ar.io](mailto:support@ar.io) for more information.
       
       @param parameters SDK options in the JSON format.
       @return
       - 0: Success.
       - &ne;0: Failure.
       */
      virtual int setParameters(const char* parameters) = 0;
//            virtual int getParameters(const char* key, any_document_t& result) = 0;
 
      /**
       Queries the online status of the specified user(s).
       
       - Online: The user has logged in the AR RTM system.
       - Offline: The user has logged out of the AR RTM system.
       
       The SDK returns the result by the \ref ar::rtm::IRtmServiceEventHandler::onQueryPeersOnlineStatusResult "onQueryPeersOnlineStatusResult" callback.
       
       @param peerIds[] An array of the specified user IDs.
       @param peerCount Length of the list.
       @param requestId The unique ID of this request.
       @return
       - 0: Success.
       - &ne;0: Failure. See #QUERY_PEERS_ONLINE_STATUS_ERR for the error codes.
       */
      virtual int queryPeersOnlineStatus(const char* peerIds[], int peerCount, long long &requestId) = 0;

      /**
       Subscribes to the online status of the specified user(s)

       The SDK returns the result by the \ref ar::rtm::IRtmServiceEventHandler::onSubscriptionRequestResult "onSubscriptionRequestResult" callback.
       
       - When the method call succeeds, the SDK returns the \ref ar::rtm::IRtmServiceEventHandler::onPeersOnlineStatusChanged "onPeersOnlineStatusChanged" callback to report the online status of peers, to whom you subscribe.
       - When the online status of the peers, to whom you subscribe, changes, the SDK returns the \ref ar::rtm::IRtmServiceEventHandler::onPeersOnlineStatusChanged "onPeersOnlineStatusChanged" callback to report whose online status has changed.
       - If the online status of the peers, to whom you subscribe, changes when the SDK is reconnecting to the server, the SDK returns the \ref ar::rtm::IRtmServiceEventHandler::onPeersOnlineStatusChanged "onPeersOnlineStatusChanged" callback to report whose online status has changed when successfully reconnecting to the server.
       
       @note
       - When you log out of the AR RTM system, all the status that you subscribe to will be cleared. To keep the original subscription after you re-log in the system, you need to redo the whole subscription process.
       - When the SDK reconnects to the server from the state of being interupted, the SDK automatically subscribes to the peers and states before the interruption without human intervention.

       @param peerIds An array of the specified user IDs.
       @param peerCount Length of the peer list.
       @param requestId The unique ID of this request.
       @return
       - 0: Success.
       - &ne;0: Failure. See #PEER_SUBSCRIPTION_STATUS_ERR for the error codes.
       */
      virtual int subscribePeersOnlineStatus(const char* peerIds[], int peerCount, long long &requestId) = 0;

      /**
       Unsubscribes from the online status of the specified user(s).

       The SDK returns the result by the \ref ar::rtm::IRtmServiceEventHandler::onSubscriptionRequestResult "onSubscriptionRequestResult" callback.

       @param peerIds An array of the specified user IDs.
       @param peerCount Length of the peer list.
       @param requestId The unique ID of this request.
       @return
       - 0: Success.
       - &ne;0: Failure. See #PEER_SUBSCRIPTION_STATUS_ERR for the error codes.
       */
      virtual int unsubscribePeersOnlineStatus(const char* peerIds[], int peerCount, long long &requestId) = 0;

      /**
       Gets a list of the peers, to whose specific status you have subscribed.

       The SDK returns the result by the \ref ar::rtm::IRtmServiceEventHandler::onQueryPeersBySubscriptionOptionResult "onQueryPeersBySubscriptionOptionResult" callback.

       @param option The status type, to which you have subscribed. See #PEER_SUBSCRIPTION_OPTION.
       @param requestId The unique ID of this request.
       @return
       - 0: Success.
       - &ne;0: Failure. See #QUERY_PEERS_BY_SUBSCRIPTION_OPTION_ERR for the error codes.
       */
      virtual int queryPeersBySubscriptionOption(PEER_SUBSCRIPTION_OPTION option, long long &requestId) = 0;

      /**
       Substitutes the local user's attributes with new ones.
       
       The SDK returns the result by the \ref ar::rtm::IRtmServiceEventHandler::onSetLocalUserAttributesResult "onSetLocalUserAttributesResult" callback.
       
       @note For \ref ar::rtm::IRtmService::setLocalUserAttributes "setLocalUserAttributes", \ref ar::rtm::IRtmService::addOrUpdateLocalUserAttributes "addOrUpdateLocalUserAttributes", \ref ar::rtm::IRtmService::deleteLocalUserAttributesByKeys "deleteLocalUserAttributesByKeys" and \ref ar::rtm::IRtmService::clearLocalUserAttributes "clearLocalUserAttributes" taken together: the call frequency limit is 10 queries every five seconds.

       @param attributes The new attributes. See RtmAttribute.
       @param numberOfAttributes The number of the attributes.
       @param requestId The unique ID of this request.
       @return
       - 0: Success.
       - &ne;0: Failure. See #ATTRIBUTE_OPERATION_ERR for the error codes.
       */
      virtual int setLocalUserAttributes(const RtmAttribute* attributes, int numberOfAttributes, long long &requestId) = 0;

      /**
       Adds or updates the local user's attribute(s).
       
       This method updates the local user's attribute(s) if it finds that the attribute(s) has/have the same key(s), or adds attribute(s) to the local user if it does not.
       
       The SDK returns the result by the \ref ar::rtm::IRtmServiceEventHandler::onAddOrUpdateLocalUserAttributesResult "onAddOrUpdateLocalUserAttributesResult" callback.
       
       @note For \ref ar::rtm::IRtmService::setLocalUserAttributes "setLocalUserAttributes", \ref ar::rtm::IRtmService::addOrUpdateLocalUserAttributes "addOrUpdateLocalUserAttributes", \ref ar::rtm::IRtmService::deleteLocalUserAttributesByKeys "deleteLocalUserAttributesByKeys" and \ref ar::rtm::IRtmService::clearLocalUserAttributes "clearLocalUserAttributes" taken together: the call frequency limit is 10 queries every five seconds.

       @param attributes The attrubutes to be added or updated. See RtmAttribute.
       @param numberOfAttributes The number of the attributes.
       @param requestId The unique ID of this request.
       @return
       - 0: Success.
       - &ne;0: Failure. See #ATTRIBUTE_OPERATION_ERR for the error codes.
       */
      virtual int addOrUpdateLocalUserAttributes(const RtmAttribute* attributes, int numberOfAttributes, long long &requestId) = 0;

      /**
       Deletes the local user's attributes by attribute keys.
       
       The SDK returns the result by the \ref ar::rtm::IRtmServiceEventHandler::onDeleteLocalUserAttributesResult "onDeleteLocalUserAttributesResult" callback.
       
       @note For \ref ar::rtm::IRtmService::setLocalUserAttributes "setLocalUserAttributes", \ref ar::rtm::IRtmService::addOrUpdateLocalUserAttributes "addOrUpdateLocalUserAttributes", \ref ar::rtm::IRtmService::deleteLocalUserAttributesByKeys "deleteLocalUserAttributesByKeys" and \ref ar::rtm::IRtmService::clearLocalUserAttributes "clearLocalUserAttributes" taken together: the call frequency limit is 10 queries every five seconds.

       @param attributeKeys An array of the attribute keys to be deleted.
       @param numberOfKeys Number of the attribute keys.
       @param requestId The unique ID of this request.
       @return
       - 0: Success.
       - &ne;0: Failure. See #ATTRIBUTE_OPERATION_ERR for the error codes.
       */
      virtual int deleteLocalUserAttributesByKeys(const char* attributeKeys[], int numberOfKeys, long long &requestId) = 0;
        
      /**
       Clears all attributes of the local user.
       
       The SDK returns the result by the \ref ar::rtm::IRtmServiceEventHandler::onClearLocalUserAttributesResult "onClearLocalUserAttributesResult" callback.
       
       @note For \ref ar::rtm::IRtmService::setLocalUserAttributes "setLocalUserAttributes", \ref ar::rtm::IRtmService::addOrUpdateLocalUserAttributes "addOrUpdateLocalUserAttributes", \ref ar::rtm::IRtmService::deleteLocalUserAttributesByKeys "deleteLocalUserAttributesByKeys" and \ref ar::rtm::IRtmService::clearLocalUserAttributes "clearLocalUserAttributes" taken together: the call frequency limit is 10 queries every five seconds.

       @param requestId The unique ID of this request.
       @return
       - 0: Success.
       - &ne;0: Failure. See #ATTRIBUTE_OPERATION_ERR for the error codes.
       */
      virtual int clearLocalUserAttributes(long long &requestId) = 0;
  
      /**
       Gets all attributes of a specified user.
       
       The SDK returns the result by the \ref ar::rtm::IRtmServiceEventHandler::onGetUserAttributesResult "onGetUserAttributesResult" callback.
       
       @note For \ref ar::rtm::IRtmService::getUserAttributes "getUserAttributes" and \ref ar::rtm::IRtmService::getUserAttributesByKeys "getUserAttributesByKeys" taken together, the call frequency limit is 40 queries every five seconds.

       @param userId The user ID of the specified user.
       @param requestId The unique ID of this request.
       @return
       - 0: Success.
       - &ne;0: Failure. See #ATTRIBUTE_OPERATION_ERR for the error codes.
       */
      virtual int getUserAttributes(const char* userId, long long &requestId) = 0;
        
      /**
       Gets the attributes of a specified user by attribute keys.
       
       The SDK returns the result by the \ref ar::rtm::IRtmServiceEventHandler::onGetUserAttributesResult "onGetUserAttributesResult" callback.
       
       @note For \ref ar::rtm::IRtmService::getUserAttributes "getUserAttributes" and \ref ar::rtm::IRtmService::getUserAttributesByKeys "getUserAttributesByKeys" taken together, the call frequency limit is 40 queries every five seconds.

       @param userId The user ID of the specified user.
       @param attributeKeys An array of the attribute keys.
       @param numberOfKeys The number of the attribute keys.
       @param requestId The unique ID of this request.
       @return
       - 0: Success.
       - &ne;0: Failure. See #ATTRIBUTE_OPERATION_ERR for the error codes.
       */
      virtual int getUserAttributesByKeys(const char* userId, const char* attributeKeys[], int numberOfKeys, long long &requestId) = 0;
        
      /**
       Resets the attributes of a specified channel.

       The SDK returns the result by the \ref ar::rtm::IRtmServiceEventHandler::onSetChannelAttributesResult "onSetChannelAttributesResult" callback.
       
       @note
       - You do not have to join the specified channel to update its attributes.
       - The attributes of a channel will be cleared if the channel remains empty (has no members) for a couple of minutes.
       - If more than one user can update the channel attributes, then AR recommends calling \ref ar::rtm::IRtmService::getChannelAttributes "getChannelAttributes" to update the cache before calling this method.
       - For \ref ar::rtm::IRtmService::setChannelAttributes "setChannelAttributes", \ref ar::rtm::IRtmService::addOrUpdateChannelAttributes "addOrUpdateChannelAttributes", \ref ar::rtm::IRtmService::deleteChannelAttributesByKeys "deleteChannelAttributesByKeys" and \ref ar::rtm::IRtmService::clearChannelAttributes "clearChannelAttributes" taken together: the call frequency limit is 10 queries every five seconds.
       
       @param channelId The channel ID of the specified channel.
       @param attributes An array of channel attributes. See \ref ar::rtm::IRtmChannelAttribute "IRtmChannelAttribute". 
       @param numberOfAttributes The total number of the channel attributes.
       @param options Options for this attribute operation. See ChannelAttributeOptions.
       @param requestId The unique ID of this request.
       @return
       - 0: Success.
       - &ne;0: Failure. See #ATTRIBUTE_OPERATION_ERR for the error codes.
       */
      virtual int setChannelAttributes(const char* channelId, const IRtmChannelAttribute* attributes[], int numberOfAttributes, const ChannelAttributeOptions &options, long long &requestId) = 0;
      
      /**
       Adds or updates the attribute(s) of a specified channel.

       This method updates the specified channel's attribute(s) if it finds that the attribute(s) has/have the same key(s), or adds attribute(s) to the channel if it does not.

       The SDK returns the result by the \ref ar::rtm::IRtmServiceEventHandler::onAddOrUpdateChannelAttributesResult "onAddOrUpdateChannelAttributesResult" callback.
       
       @note
       - You do not have to join the specified channel to update its attributes.
       - The attributes of a channel will be cleared if the channel remains empty (has no members) for a couple of minutes.
       - If more than one user can update the channel attributes, then AR recommends calling \ref ar::rtm::IRtmService::getChannelAttributes "getChannelAttributes" to update the cache before calling this method.
       - For \ref ar::rtm::IRtmService::setChannelAttributes "setChannelAttributes", \ref ar::rtm::IRtmService::addOrUpdateChannelAttributes "addOrUpdateChannelAttributes", \ref ar::rtm::IRtmService::deleteChannelAttributesByKeys "deleteChannelAttributesByKeys" and \ref ar::rtm::IRtmService::clearChannelAttributes "clearChannelAttributes" taken together: the call frequency limit is 10 queries every five seconds.

       @param channelId The channel ID of the specified channel.
       @param attributes An array of channel attributes. See \ref ar::rtm::IRtmChannelAttribute "IRtmChannelAttribute".
       @param numberOfAttributes The total number of the channel attributes.
       @param options Options for this attribute operation. See ChannelAttributeOptions.
       @param requestId The unique ID of this request.
       @return
       - 0: Success.
       - &ne;0: Failure. See #ATTRIBUTE_OPERATION_ERR for the error codes.
       */
      virtual int addOrUpdateChannelAttributes(const char* channelId, const IRtmChannelAttribute* attributes[], int numberOfAttributes, const ChannelAttributeOptions &options, long long &requestId) = 0;
        
      /**
       Deletes the attributes of a specified channel by attribute keys.

       The SDK returns the result by the \ref ar::rtm::IRtmServiceEventHandler::onDeleteChannelAttributesResult "onDeleteChannelAttributesResult" callback.
       
       @note
       - You do not have to join the specified channel to update its attributes.
       - The attributes of a channel will be cleared if the channel remains empty (has no members) for a couple of minutes.
       - If more than one user can update the channel attributes, then AR recommends calling \ref ar::rtm::IRtmService::getChannelAttributes "getChannelAttributes" to update the cache before calling this method.
       - For \ref ar::rtm::IRtmService::setChannelAttributes "setChannelAttributes", \ref ar::rtm::IRtmService::addOrUpdateChannelAttributes "addOrUpdateChannelAttributes", \ref ar::rtm::IRtmService::deleteChannelAttributesByKeys "deleteChannelAttributesByKeys" and \ref ar::rtm::IRtmService::clearChannelAttributes "clearChannelAttributes" taken together: the call frequency limit is 10 queries every five seconds.

       @param channelId The channel ID of the specified channel.
       @param attributeKeys An array of channel attribute keys.
       @param numberOfKeys The total number of the channel attributes.
       @param options Options for this attribute operation. See ChannelAttributeOptions.
       @param requestId The unique ID of this request.
       @return
       - 0: Success.
       - &ne;0: Failure. See #ATTRIBUTE_OPERATION_ERR for the error codes.
       */
      virtual int deleteChannelAttributesByKeys(const char* channelId, const char* attributeKeys[], int numberOfKeys, const ChannelAttributeOptions &options, long long &requestId) = 0;
        
      /**
       Clears all attributes of a specified channel.

       The SDK returns the result by the \ref ar::rtm::IRtmServiceEventHandler::onClearChannelAttributesResult "onClearChannelAttributesResult" callback.
       
       @note
       - You do not have to join the specified channel to clear its attributes.
       - The attributes of a channel will be cleared if the channel remains empty (has no members) for a couple of minutes.
       - For \ref ar::rtm::IRtmService::setChannelAttributes "setChannelAttributes", \ref ar::rtm::IRtmService::addOrUpdateChannelAttributes "addOrUpdateChannelAttributes", \ref ar::rtm::IRtmService::deleteChannelAttributesByKeys "deleteChannelAttributesByKeys" and \ref ar::rtm::IRtmService::clearChannelAttributes "clearChannelAttributes" taken together: the call frequency limit is 10 queries every five seconds.

       @param channelId The channel ID of the specified channel.
       @param options Options for this attribute operation. See ChannelAttributeOptions.
       @param requestId The unique ID of this request.
       @return
       - 0: Success.
       - &ne;0: Failure. See #ATTRIBUTE_OPERATION_ERR for the error codes.
       */
      virtual int clearChannelAttributes(const char* channelId, const ChannelAttributeOptions &options, long long &requestId) = 0;
        
      /**
       Gets all attributes of a specified channel.
       
       The SDK returns the result by the \ref ar::rtm::IRtmServiceEventHandler::onGetChannelAttributesResult "onGetChannelAttributesResult" callback.
       
       @note
       - You do not have to join the specified channel to get its attributes.
       - For \ref ar::rtm::IRtmService::getChannelAttributes "getChannelAttributes" and \ref ar::rtm::IRtmService::getChannelAttributesByKeys "getChannelAttributesByKeys" taken together, the call frequency limit is 10 queries every five seconds.

       @param channelId The channel ID of the specified channel.
       @param requestId The unique ID of this request.
       @return
       - 0: Success.
       - &ne;0: Failure. See #ATTRIBUTE_OPERATION_ERR for the error codes.
       */
      virtual int getChannelAttributes(const char* channelId, long long &requestId) = 0;
        
      /**
       Gets the attributes of a specified channel by attribute keys.
       
       The SDK returns the result by the \ref ar::rtm::IRtmServiceEventHandler::onGetChannelAttributesResult "onGetChannelAttributesResult" callback.
       
       @note
       - You do not have to join the specified channel to get its attributes.
       - For \ref ar::rtm::IRtmService::getChannelAttributes "getChannelAttributes" and \ref ar::rtm::IRtmService::getChannelAttributesByKeys "getChannelAttributesByKeys" taken together, the call frequency limit is 10 queries every five seconds.

       @param channelId The channel ID of the specified channel.
       @param attributeKeys An array of attribute keys.
       @param numberOfKeys The total number of the attribute keys.
       @param requestId The unique ID of this request.
       @return
       - 0: Success.
       - &ne;0: Failure. See #ATTRIBUTE_OPERATION_ERR for the error codes.
       */
      virtual int getChannelAttributesByKeys(const char* channelId, const char* attributeKeys[], int numberOfKeys, long long &requestId) = 0;
      
      /**
       Gets the member count of specified channel(s).
       
       The SDK returns the result by the \ref ar::rtm::IRtmServiceEventHandler::onGetChannelMemberCountResult "onGetChannelMemberCountResult" callback.

       @note
       - The call frequency limit for this method is one query per second.
       - We do not support getting the member counts of more than 32 channels in one method call.
       - You do not have to join the specified channel(s) to call this method. 

       @param channelIds An array of the specified channel ID(s).
       @param channelCount The total number of the channels.
       @param requestId The unique ID of this request.
       @return
       - 0: Success.
       - &ne;0: Failure. See #GET_CHANNEL_MEMBER_COUNT_ERR_CODE for the error codes.
       */
      virtual int getChannelMemberCount(const char* channelIds[], int channelCount, long long &requestId) = 0;
        
      /**
       Specifies the default path to the SDK log file.
       
       @note
       - Ensure that the directory holding the log file exists and is writable.
       - Ensure that you call this method immediately after calling the \ref ar::rtm::IRtmService::initialize "initialize" method to initialize an \ref ar::rtm::IRtmService "IRtmService" instance, otherwise the output log may be incomplete.
       - For the Linux C++ platform, <i>agorartm.log</i> is in the current working directory.
       
       @param logfile The absolute file path to the log file. The string of @p logfile is in UTF-8. 
       @return
       - 0: Success.
       - &ne;0: Failure.
       */
      virtual int setLogFile(const char* logfile) = 0;
        
      /**
       Sets the output log level of the SDK.
       
       You can use one or a combination of the filters. The log level follows the sequence of OFF, CRITICAL, ERROR, WARNING, and INFO. Choose a level to see the logs preceding that level. If, for example, you set the log level to WARNING, you see the logs within levels CRITICAL, ERROR, and WARNING.

       @param filter The log filter level. See #LOG_FILTER_TYPE.
       @return
       - 0: Success.
       - &ne;0: Failure.
       */
      virtual int setLogFilter(LOG_FILTER_TYPE filter) = 0;
      
      /**
       Sets the log file size in KB.
       
       The SDK has two log files, each with a default size of 512 KB. If you set fileSizeInBytes as 1024 KB, the SDK outputs log files with a total maximum size of 2 MB.
       
       @note File size settings of less than 512 KB or greater than 10 MB will not take effect.

       @param fileSizeInKBytes The SDK log file size (KB).
       @return
       - 0: Success.
       - &ne;0: Failure.
       */
      virtual int setLogFileSize(int fileSizeInKBytes) = 0;
    };
////////////////////////////////////////////////////////
/** \addtogroup createRtmService
@{
*/
////////////////////////////////////////////////////////
      
/**
Creates an \ref ar::rtm::IRtmService "IRtmService" instance.

@note When you no longer need an \ref ar::rtm::IRtmService "IRtmService" instance, ensure that you call the \ref ar::rtm::IRtmService::release "release" method to release all resources that it uses.

@return An \ref ar::rtm::IRtmService "IRtmService" instance.
*/
ARM_API IRtmService* ARM_CALL createRtmService();

////////////////////////////////////////////////////////
/** @} */
////////////////////////////////////////////////////////

////////////////////////////////////////////////////////
/** \addtogroup getRtmSdkVersion
@{
*/
////////////////////////////////////////////////////////

/**
Gets the version of the AR RTM SDK.

@return The version of the AR RTM SDK.
*/
ARM_API const char* ARM_CALL getRtmSdkVersion();

////////////////////////////////////////////////////////
/** @} */
////////////////////////////////////////////////////////
  }
}
