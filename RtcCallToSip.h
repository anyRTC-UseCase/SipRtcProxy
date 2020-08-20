#ifndef __RTC_CALL_TO_SIP_H__
#define __RTC_CALL_TO_SIP_H__
#include "SipCall.h"
#include "RtcCall.h"

class RtcCallToSipEvent
{
public:
	RtcCallToSipEvent(void) {};
	virtual ~RtcCallToSipEvent(void) {};

	// 呼叫结束
	virtual void OnRtcCallToSipClosed(const std::string&strCallerId, int nCode) = 0;
};

/** RtcCallToSip类
 * 实现Rtc主动呼叫Sip的数据互通
 * 
 */
class RtcCallToSip : public RtcCall, public SipCallEvent
{
public:
	RtcCallToSip(RtcCallToSipEvent&callback);
	virtual ~RtcCallToSip(void);

	void SetCallerId(const std::string&strCallerId) { str_caller_id_ = strCallerId; };
	const std::string&SipAccount() { return str_sip_account_; };

	void InitSipAccount(const std::string&strSvrIp, int nPort, const std::string&strAccount, const std::string&strPwd);
	void StartTask(const std::string&strAppId, const std::string&strChanId, const std::string&strSipNumber, const std::string&strSipData);
	void StopTask();

	//* For RtcCall
	virtual void OnRtcCallAudioMixData(const char*pData, int nLen, int nSampleHz, int nChannels, uint32_t timestamp);

	//* For SipCallEvent
	virtual void OnRegState(bool Ok);
	virtual void OnCallOk();
	virtual void OnCallHangUp();
	virtual void OnCallAudioData(const char*pData, int nLen, int nSampleHz, int nChannels);

private:
	SipCall	*sip_call_;
	RtcCallToSipEvent &callback_;
	bool	task_started_;
	std::string str_caller_id_;
	std::string str_sip_svr_;
	std::string str_sip_account_;
	std::string str_sip_number_;
	std::string str_sip_data_;
};

#endif	// __RTC_CALL_TO_SIP_H__

