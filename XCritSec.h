// XCritSec.h: interface for the XCritSec class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __XCRITSEC_H__
#define __XCRITSEC_H__

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#else
#include <semaphore.h>
#include <pthread.h>
#endif

class XCritSec  
{
public:
	XCritSec(void);
	virtual ~XCritSec(void);
public:
    void Lock(void);
    void UnLock(void);
protected:
#ifdef _WIN32
    CRITICAL_SECTION m_CritSec;
#else
	pthread_mutex_t		m_pMutex;
#endif
};

#endif 