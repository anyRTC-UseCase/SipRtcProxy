#ifndef __I_AR_RTM_LOG_H__
#define __I_AR_RTM_LOG_H__
#include "IArRtmService.h"

ARM_API void ARM_CALL ArRtm_OpenRtcLog(const char*strPath, int nLevel, int nMaxFileSize);
ARM_API void ARM_CALL ArRtm_CloseRtcLog();

#endif	//__I_AR_RTM_LOG_H__