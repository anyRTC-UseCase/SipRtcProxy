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
#include "IArRtmLog.h"

#define SVR_VERSION "1.0.0.2.20200922"

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

	sipRtcMgr.SetRtcRtmAppId(conf.GetValue("global", "appid"));
	sipRtcMgr.SetSipSvr(conf.GetValue("global", "sip_svr"));
	sipRtcMgr.SetRtcSvrPort(conf.GetValue("rtc", "ip"), conf.GetIntVal("rtc", "port"));
	sipRtcMgr.SetRtmSvrPort(conf.GetValue("rtm", "ip"), conf.GetIntVal("rtm", "port"));
	sipRtcMgr.SetPstnSvr(conf.GetValue("pstn", "ip"), conf.GetValue("pstn", "prefix"));

	std::string strSipSvrIp;
	int nSipPort = 5060;
	{//* 读取Sip的配置
		std::string strSvr = conf.GetValue("global", "sip_svr");
		// Sip库初始化
		SipCall::Init(conf.GetIntVal("global", "sip_local_port", 5086));

		if (conf.GetIntVal("proxy", "on") != 0) {// SipProxy - 打开
			std::string strProxyAcc = conf.GetValue("proxy", "sip_account");
			std::string strProxyPwd = conf.GetValue("proxy", "sip_pwd");
			std::string strRtmAccount = conf.GetValue("proxy", "rtm_account");
			sipRtcMgr.StartSipProxy(strSvr, strProxyAcc, strProxyPwd, strRtmAccount);
		}
	}

	//* 初始化Rtc推拉流库
	initARtSEngine();

	ArRtm_OpenRtcLog("./sip_rtm.log/ar_rtm.log", 2, 10*1024);

	if (conf.GetIntVal("rtm2sip", "on") != 0) {// RtmToSip - 开启
		sipRtcMgr.StartRtm2Sip(conf.GetValue("rtm2sip", "acc_rule"));
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

	ArRtm_CloseRtcLog();

	return 0;
}

