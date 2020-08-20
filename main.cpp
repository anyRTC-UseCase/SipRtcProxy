// SipRtcProxy.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "config.h"
#include "XUtil.h"
#include "RtcLog.h"
#include "SipRtcMgr.h"

int main(int argc, char*argv[])
{
	ConfigSet	conf;
	if (argc > 1)
		conf.LoadFromFile(argv[1]);
	else
	{
		std::cout << "Error: Please usage: $0 {conf_path} " << std::endl;
		std::cout << "Please enter any key to exit ..." << std::endl;
		getchar();
		exit(0);
	}

	SipRtcMgr sipRtcMgr;

	std::string strSipSvrIp;
	int nSipPort = 5060;
	{//* 读取Sip的配置
		std::string strSvr = conf.GetValue("global", "sip_svr");
		std::string strAccPrefix = conf.GetValue("sip", "acc_prefix");
		std::string strAccRule = conf.GetValue("sip", "acc_rule");
		std::string strAccPassword = conf.GetValue("sip", "acc_pwd");
		size_t pos = strSvr.find(":");
		if (pos != std::string::npos) {
			strSipSvrIp = strSvr.substr(0, pos);
			std::string strPort = strSvr.substr(pos + 1);
			nSipPort = atoi(strPort.c_str());
		}
		else {
			strSipSvrIp = strSvr;
		}
		if (!sipRtcMgr.SetSipAccount(strSipSvrIp.c_str(), nSipPort, strAccPrefix.c_str(), strAccRule.c_str(), strAccPassword.c_str()))
		{
			return -1;
		}

		SipCall::Init();

		if (conf.GetIntVal("proxy", "on") != 0) {
			std::string strProxyAcc = conf.GetValue("proxy", "account");
			std::string strProxyPwd = conf.GetValue("proxy", "pwd");
			sipRtcMgr.StartSipProxy(strSipSvrIp, strProxyAcc, strProxyPwd);
		}
	}

	sipRtcMgr.SetRtcRtmAppId(conf.GetValue("global", "appid"));
	sipRtcMgr.SetRtcSvrPort(conf.GetValue("rtc", "ip"), conf.GetIntVal("rtc", "port"));
	sipRtcMgr.SetRtmSvrPort(conf.GetValue("rtm", "ip"), conf.GetIntVal("rtm", "port"));

	initARtSEngine();

	if (conf.GetIntVal("ivr", "on") != 0) {
		sipRtcMgr.StartIvr(conf.GetValue("ivr", "rtm_account"), conf.GetValue("ivr", "sip_account"));
	}
	

	while (1)
	{
		if (!sipRtcMgr.ProcessMsg()) {
			break;
		}

		XSleep(1);
	}

	sipRtcMgr.StopAll();

	SipCall::Deinit();
	deinitARtSEngine();

	return 0;
}

