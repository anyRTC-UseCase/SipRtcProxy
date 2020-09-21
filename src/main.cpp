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
#include <iostream>
#include "config.h"
#include "XUtil.h"
#include "RtcLog.h"
#include "SipRtcMgr.h"

#define SVR_VERSION "1.0.0.1.20200827"

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
	printf("****** SRProxy version: %s ******\r\n", SVR_VERSION);

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
		{// 配置sip账号，用于RtcToSip呼叫
			sipRtcMgr.StopAll();
			return -1;
		}

		// Sip库初始化
		SipCall::Init();

		if (conf.GetIntVal("proxy", "on") != 0) {// SipProxy - 打开
			std::string strProxyAcc = conf.GetValue("proxy", "account");
			std::string strProxyPwd = conf.GetValue("proxy", "pwd");
			sipRtcMgr.StartSipProxy(strSvr, strProxyAcc, strProxyPwd);
		}
	}

	sipRtcMgr.SetRtcRtmAppId(conf.GetValue("global", "appid"));
	sipRtcMgr.SetRtcSvrPort(conf.GetValue("rtc", "ip"), conf.GetIntVal("rtc", "port"));
	sipRtcMgr.SetRtmSvrPort(conf.GetValue("rtm", "ip"), conf.GetIntVal("rtm", "port"));

	//* 初始化Rtc推拉流库
	initARtSEngine();

	if (conf.GetIntVal("ivr", "on") != 0) {// IVR - 开启
		sipRtcMgr.StartIvr(conf.GetValue("ivr", "rtm_account"), conf.GetValue("ivr", "sip_account"));
	}
	

	while (1)
	{// 主线程
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

