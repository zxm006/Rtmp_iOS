#include "stdafx.h"
#include "CritSec.h"

//---------------------------------------------------------------------------------------
KCritSec::KCritSec(void)
{
#ifdef WIN32
    InitializeCriticalSection(&m_CritSec);
#else
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
#ifdef _LINUX
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
#endif
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&m_pMutex, &attr);
    pthread_mutexattr_destroy(&attr);
#endif
}

//---------------------------------------------------------------------------------------
KCritSec::~KCritSec(void)
{
#ifdef WIN32
    DeleteCriticalSection(&m_CritSec);
#else
	pthread_mutex_destroy(&m_pMutex);
#endif
}

//---------------------------------------------------------------------------------------
void KCritSec::Lock(void)
{
#ifdef WIN32
    EnterCriticalSection(&m_CritSec);
#else
	(void)pthread_mutex_lock(&m_pMutex);
#endif
}

//---------------------------------------------------------------------------------------
void KCritSec::Unlock(void)
{
#ifdef WIN32
    LeaveCriticalSection(&m_CritSec);
#else
	pthread_mutex_unlock(&m_pMutex);
#endif
}

