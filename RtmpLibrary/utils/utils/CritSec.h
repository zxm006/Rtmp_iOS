#pragma once

#ifndef WIN32
#include <pthread.h>
#endif

#ifdef WIN32
#include <windows.h>
#endif

//---------------------------------------------------------------------------------------
//临界区类:用于设置线程临界区
class KCritSec  
{
public:
	KCritSec();
	virtual ~KCritSec();

	// 锁定临界区
    void Lock(void);

	// 解锁临界区
    void Unlock(void);

protected:

#ifdef WIN32
    CRITICAL_SECTION m_CritSec;
#else
	pthread_mutex_t m_pMutex;
#endif
};
