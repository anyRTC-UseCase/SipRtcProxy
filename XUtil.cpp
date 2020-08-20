// XUtil.cpp: implementation of the XUtil class.
//
//////////////////////////////////////////////////////////////////////
#include "XUtil.h"
#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#include <mmsystem.h>
#include <sys/timeb.h>

#ifdef NETEC_ARM
#pragma comment(lib,"mmtimer.lib")
#else
#pragma comment(lib,"winmm.lib")
#endif
#else
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <semaphore.h>
#include <pthread.h>
#include <assert.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#endif

static const int64_t kNumMillisecsPerSec = INT64_C(1000);
static const int64_t kNumMicrosecsPerSec = INT64_C(1000000);
static const int64_t kNumNanosecsPerSec = INT64_C(1000000000);


int64_t TimeUTCMicros() {
#ifdef _WIN32
	struct _timeb time;
	_ftime(&time);
	// Convert from second (1.0) and milliseconds (1e-3).
	return (static_cast<int64_t>(time.time) * kNumMicrosecsPerSec +
		static_cast<int64_t>(time.millitm) * 1000);
#else
	struct timeval time;
	gettimeofday(&time, nullptr);
	// Convert from second (1.0) and microsecond (1e-6).
	return (static_cast<int64_t>(time.tv_sec) * kNumMicrosecsPerSec +
		time.tv_usec);
#endif
	return 0;
}

int64_t TimeUTCMillis() {
	return TimeUTCMicros() / 1000;
}

uint32_t XGetTimestamp(void)
{
#ifdef _WIN32
	return ::timeGetTime();//毫秒
#else
	struct timeval now;
	gettimeofday(&now, NULL);
	return now.tv_sec * 1000 + now.tv_usec / 1000;
#endif
}

int64_t XGetUtcTimestamp(void)
{
	return TimeUTCMillis();
}


uint16_t XGetLastSequence(uint16_t usSequence1,uint16_t usSequence2)
{
	uint16_t usLastSequence=usSequence2;
	if ((usSequence1>usSequence2 && (uint16_t)(usSequence1-usSequence2)<32768) ||
		(usSequence1<usSequence2 && (uint16_t)(usSequence2-usSequence1)>32768))
	{
		usLastSequence=usSequence1;
	}

	return usLastSequence;
}

uint32_t XGenerateSSRC(void)
{
#ifdef _WIN32
	LARGE_INTEGER frequence, privious;
	if(!QueryPerformanceFrequency( &frequence))//取高精度运行计数器的频率
	{
		return timeGetTime();
	}

	if (!QueryPerformanceCounter( &privious ))
	{
		return timeGetTime();
	}

	DWORD dwRet = (DWORD)(1000000 * privious.QuadPart / frequence.QuadPart ); //换算到微秒数

	return dwRet;//微秒
#else
	struct timeval now;
	gettimeofday(&now,NULL);
	return now.tv_sec*1000+now.tv_usec; 
#endif
}

void XSleep(uint32_t ulMS)
{
#ifdef _WIN32
	Sleep(ulMS);
#else
	usleep(ulMS*1000);
#endif
}

int XSplitChar(const std::string& source, char delimiter,
	std::vector<std::string>* fields) {
	fields->clear();
	size_t last = 0;
	for (size_t i = 0; i < source.length(); ++i) {
		if (source[i] == delimiter) {
			fields->push_back(source.substr(last, i - last));
			last = i + 1;
		}
	}
	fields->push_back(source.substr(last, source.length() - last));
	return fields->size();
}