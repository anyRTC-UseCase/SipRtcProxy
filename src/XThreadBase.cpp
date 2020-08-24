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
#include "XThreadBase.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

XThreadBase::XThreadBase()
{
#ifndef WIN32
	sem_init(&m_semWaitForStop,0,1);
	m_thread = 0;
	m_bThreadStopped=true;
#else
	m_dwThreadID=0;
	m_hThread=NULL;
	m_evStop = CreateEvent(NULL,TRUE,TRUE,NULL);
	SetEvent(m_evStop);
#endif
}

XThreadBase::~XThreadBase()
{
#ifndef WIN32
	sem_destroy(&m_semWaitForStop);
#else
	CloseHandle(m_evStop);
#endif
}

bool XThreadBase::StartThread(int nPrio)
{
#ifndef WIN32
	if (pthread_create(&m_thread,NULL,InitThreadProc,(void*)this)!=0)
		return false;
	//sem_wait(&m_semWaitForStop);
	m_bThreadStopped=false;
	return true;
#else
	//如果线程已经创建，则不需要再创建
	if (!m_hThread){ 
		//创建并启动线程函数
		m_hThread = CreateThread(
					NULL,			//the handle cannot be inherited. 
                    0,				//the default Thread Stack Size is set
                    InitThreadProc,	//线程函数
                    this,			//线程函数的参数
                    0,				//使线程函数创建完后立即启动
                    &m_dwThreadID	//receives the thread identifier
					);
                
        }//end if (!m_hThread...

	if (m_hThread)
		ResetEvent(m_evStop);

    if( nPrio == 0 )
	    SetThreadPriority(m_hThread,THREAD_PRIORITY_HIGHEST);
    else
	    SetThreadPriority(m_hThread, THREAD_PRIORITY_TIME_CRITICAL);

	return	m_hThread != NULL;
#endif
}

void XThreadBase::WaitForStop()
{
#ifndef WIN32
	if (m_bThreadStopped==false)
	{
		sem_wait(&m_semWaitForStop);
		m_bThreadStopped=true;
		pthread_join(m_thread,NULL);
	}
#else
		
	WaitForSingleObject(m_evStop,INFINITE);

	if (m_hThread) 
	{
		CloseHandle(m_hThread);
		m_hThread=NULL;
	}
#endif
}

unsigned long  XThreadBase::ThreadProc()
{
	ThreadProcMain();
#ifndef WIN32
	sem_post(&m_semWaitForStop);
#else
	SetEvent(m_evStop);
#endif
	return 0;
}

