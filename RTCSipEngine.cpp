#include "RTCSipEngine.h"
#include <map>
#include "RTCCorePure.h"
#include "RTCEngineImpl.h"
#include "SipCall/SipCall.h"
#include "dync/DyncPeerConnection.h"
#include "webrtc/rtc_base/logging.h"

extern int gUdpPortMin;
extern int gUdpPortMax;
typedef std::map<std::string, int> MapSipAccount;
MapSipAccount	gMapSipAccount;
MapSipAccount	gMapFxoAccount;
std::string		gStrSvrIp;
int				gSvrPort = 5060;
std::string		gStrSipPassword;
std::string		gStrFxoAddr;
std::string		gStrFxoPrefix;
std::string ApplySipAccount()
{
	MapSipAccount::iterator iter = gMapSipAccount.begin();
	while (iter != gMapSipAccount.end()) {
		if (iter->second == 0) {
			iter->second = 1;
			RTC_LOG(LS_INFO) << "ApplySipAccount: " << iter->first;
			return iter->first;
		}
		iter++;
	}

	return "";
}
void ReleaseSipAccount(const std::string&strAccount) {
	RTC_LOG(LS_INFO) << "ReleaseSipAccount: " << strAccount;
	if (gMapSipAccount.find(strAccount) != gMapSipAccount.end()) {
		gMapSipAccount[strAccount] = 0;
	}
}

std::string ApplyFxoAccount()
{
	MapSipAccount::iterator iter = gMapFxoAccount.begin();
	while (iter != gMapFxoAccount.end()) {
		if (iter->second == 0) {
			iter->second = 1;
			return iter->first;
		}
		iter++;
	}

	return "";
}
void ReleaseFxoAccount(const std::string&strAccount) {
	if (gMapFxoAccount.find(strAccount) != gMapFxoAccount.end()) {
		gMapFxoAccount[strAccount] = 0;
	}
}

void RTCSipEngine::Init(const std::string&strSipSvrIp, int nSipSvrPort)
{
	RTCCorePure::Inst().Init();
	SipCall::Init(strSipSvrIp, nSipSvrPort);
}
void RTCSipEngine::Deinit()
{
	SipCall::Deinit();
	RTCCorePure::Inst().Clear();
}

void RTCSipEngine::ConfigServerForPriCloud(const char*strSvrAddr, int nSvrPort)
{
	RTCEngineImpl::Inst().ConfigServerForPriCloud(strSvrAddr, nSvrPort);
}
void RTCSipEngine::SetLogLevel(int nLevel)
{
	rtc::LogMessage::LogToDebug((rtc::LoggingSeverity) nLevel);
}
bool RTCSipEngine::SetSipAccount(const char*strSvrIp, int nPort, const char*strPrefix, const char*strRule, const char*strPassword)
{
	if (strRule == NULL || strlen(strRule) == 0) {
		RTC_LOG(LS_ERROR) << "Sip account rule is null";
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
		RTC_LOG(LS_ERROR) << "Sip account rule need xxx-yyy";
		return false;
	}
	if (strFrom.length() > strTo.length()) {
		RTC_LOG(LS_ERROR) << "Sip account rule(xxx-yyy) is error, length of xxx need small than yyy";
		return false;
	}
	int nMinDigit = strFrom.length();
	int nFrom = atoi(strFrom.c_str());
	int nTo = atoi(strTo.c_str());
	if (nTo < nFrom) {
		RTC_LOG(LS_ERROR) << "Sip account rule(xxx-yyy) is error, yyy need bigger than xxx";
		return false;
	}
	gStrSvrIp = strSvrIp;
	gSvrPort = nPort;
	gStrSipPassword = strPassword;
	char *pAccount = new char[strlen(strPrefix) + nTo + 16];
	for (int i = nFrom; i <= nTo; i++) {
		sprintf(pAccount, "%s%.*d", strPrefix, nMinDigit, i);
		gMapSipAccount[pAccount] = 0;
	}
	delete[] pAccount;
}
bool RTCSipEngine::SetSipFxoAccount(const char*strPrefix, const char*strCount, const char*strAddr)
{
	if (strCount == NULL || strlen(strCount) == 0) {
		RTC_LOG(LS_ERROR) << "Fxo count is null";
		return false;
	}
	int nCt = atoi(strCount);
	if (nCt <= 0 || nCt > 9999) {
		RTC_LOG(LS_ERROR) << "Fxo count must(0-9999) but setting is: " << nCt;
		return false;
	}
	if (strPrefix != NULL && strlen(strPrefix) > 0) {
		gStrFxoPrefix = strPrefix;
	}
	if (strAddr != NULL && strlen(strAddr) > 0) {
		gStrFxoAddr = strAddr;
	}

	char strAccount[64];
	for (int i = 1; i <= nCt; i++) {
		sprintf(strAccount, "fxo_%d", i);
		gMapFxoAccount[strAccount] = 0;
	}
}
void RTCSipEngine::SetLocalPublicIPAddr(const char*strLocalAddr, const char*strPublicAddr)
{
	DyncPeerConnection::gStrLocalIp = strLocalAddr;
	DyncPeerConnection::gStrPublicIp = strPublicAddr;
}
bool RTCSipEngine::SetPortAllocatoer(int nPortMin, int nPortMax)
{
	if (nPortMin >= nPortMax || nPortMin < 0 || nPortMax > 65535) {
		return false;
	}
	if ((nPortMax - nPortMin) < 10) {
		return false;
	}
	gUdpPortMin = nPortMin;
	gUdpPortMax = nPortMax;
	return true;
}
