// XThreadBase.h: interface for the CAVThreadclass.
//
//////////////////////////////////////////////////////////////////////

#ifndef __XTHREADBASE_H__
#define __XTHREADBASE_H__

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#else
#include <semaphore.h>
#include <pthread.h>
#endif

class XThreadBase
{
public:
	XThreadBase(void);
	virtual ~XThreadBase(void);
public:
	bool StartThread(int nPrio=0);
	void WaitForStop(void);

#ifdef WIN32
	static DWORD WINAPI InitThreadProc(PVOID pObj){
		return	((XThreadBase*)pObj)->ThreadProc();
	}
#else
	static void* InitThreadProc(void*pObj){
		((XThreadBase*)pObj)->ThreadProc();
		return 0;
	}
#endif
	unsigned long ThreadProc(void);

protected:
	virtual void ThreadProcMain(void)=0;
protected:
#ifdef WIN32
	DWORD	m_dwThreadID;
	HANDLE	m_hThread;
	HANDLE	m_evStop;
#else
    pthread_t	m_thread;
    sem_t		m_semWaitForStop;
    bool		m_bThreadStopped;
#endif
};

#endif
