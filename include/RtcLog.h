#ifndef __RTC_LOG_H__
#define __RTC_LOG_H__
#include <stdarg.h>

#define TRACE	0		//spdlog::level::level_enum::trace
#define DBG		1		//spdlog::level::level_enum::debug
#define INF		2		//spdlog::level::level_enum::info
#define WARN	3		//spdlog::level::level_enum::warn
#define ERR		4		//spdlog::level::level_enum::err
#define CRIT	5		//spdlog::level::level_enum::critical

void OpenRtcLog(const char*strPath, int nLevel, int nMaxFileSize);
void CloseRtcLog();

void RtcLog(int nLevel, const char*strMsg);

void RtcPrintf(int nLevel, const char *fmt, ...);

void SetRtcLogFilter(int filter);

#endif	// __RTC_LOG_H__
