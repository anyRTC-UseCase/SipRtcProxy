// XCritSec.cpp: implementation of the XCritSec class.
//
//////////////////////////////////////////////////////////////////////
#include "XCritSec.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XCritSec::XCritSec(void)
{
#ifdef WIN32
    InitializeCriticalSection(&m_CritSec);
#else
#if(0)
	//linux 7.3
	pthread_mutex_init(&m_pMutex, NULL);
#else
	pthread_mutexattr_t attr;
	pthread_mutexattr_init(&attr);
#ifndef NETEC_IOS
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
#endif
	pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&m_pMutex, &attr);
	pthread_mutexattr_destroy(&attr);
#endif
#endif
}

XCritSec::~XCritSec(void)
{
#ifdef WIN32
    DeleteCriticalSection(&m_CritSec);
#else
	pthread_mutex_destroy(&m_pMutex);
#endif
}

void XCritSec::Lock(void)
{
#ifdef WIN32
    EnterCriticalSection(&m_CritSec);
#else
	(void)pthread_mutex_lock(&m_pMutex);
#endif
}

void XCritSec::UnLock(void)
{
#ifdef WIN32
    LeaveCriticalSection(&m_CritSec);
#else
	pthread_mutex_unlock(&m_pMutex);
#endif
}
