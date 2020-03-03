//#include "../videoSender/stdafx.h"
#include "Thread.h"
#include <stdio.h>

#ifdef WIN32
#include <windows.h>
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------------------
KThread::KThread()
{
#ifndef WIN32
//	sem_init(&m_semWaitForStop,0,1);
    m_semWaitForStop=sem_open("m_semWaitForStop",O_CREAT,0,1);
    
    m_thread = 0;
    m_bThreadStopped=true;
#else
    m_dwThreadID=0;
	m_hThread=NULL;
	m_evStop = CreateEvent(NULL,TRUE,TRUE,NULL);
	SetEvent(m_evStop);
#endif
}

//---------------------------------------------------------------------------------------
KThread::~KThread()
{
#ifndef WIN32
	sem_close(m_semWaitForStop);
#else
	CloseHandle(m_evStop);
#endif
}

//---------------------------------------------------------------------------------------
bool KThread::StartThread()
{
    //LOGI("KThread::StartThread()");
#ifndef WIN32
	if (pthread_create(&m_thread,NULL,InitThreadProc,(void*)this)!=0)
		return false;
	sem_wait(m_semWaitForStop);
	m_bThreadStopped=false;
	return true;
#else
	//»Áπ˚œﬂ≥Ã“—æ≠¥¥Ω®£¨‘Ú≤ª–Ë“™‘Ÿ¥¥Ω®
	if (!m_hThread){ 
		//¥¥Ω®≤¢∆Ù∂Øœﬂ≥Ã∫Ø ˝
		m_hThread = CreateThread(
					NULL,			//the handle cannot be inherited. 
                    0,				//the default Thread Stack Size is set
                    InitThreadProc,	//œﬂ≥Ã∫Ø ˝
                    this,			//œﬂ≥Ã∫Ø ˝µƒ≤Œ ˝
                    0,				// πœﬂ≥Ã∫Ø ˝¥¥Ω®ÕÍ∫Û¡¢º¥∆Ù∂Ø
                    &m_dwThreadID	//receives the thread identifier
					);
                
        }//end if (!m_hThread...

	if (m_hThread)
		ResetEvent(m_evStop);

	return	m_hThread != NULL;
#endif
}

//---------------------------------------------------------------------------------------
void KThread::WaitForStop()
{
#ifndef WIN32
	if (m_bThreadStopped==false)
	{
		sem_wait(m_semWaitForStop);
		m_bThreadStopped=true;
		pthread_join(m_thread,NULL);
	}
#else
		
	WaitForSingleObject(m_evStop,INFINITE);

	// ∑µªÿœﬂ≥Ãæ‰±˙
	HANDLE hThread = (HANDLE)InterlockedExchange((LONG *)&m_hThread, 0);
	if (hThread) {
		// µ»¥˝œﬂ≥Ã÷’÷π
		WaitForSingleObject(hThread, INFINITE);
		// πÿ±’œﬂ≥Ãæ‰±˙
		CloseHandle(hThread);
	}// end if (hThread...
#endif
}

//---------------------------------------------------------------------------------------
unsigned long  KThread::ThreadProc()
{
    //LOGI("KThread::ThreadProc()");
	ThreadProcMain();
#ifndef WIN32
    sem_post(m_semWaitForStop);
    sem_post(m_semWaitForStop);
#else
	SetEvent(m_evStop);
#endif
	return 0;
}

