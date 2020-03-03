//#include "../videoSender/stdafx.h"
#include "Timer.h"
#include "AutoLock.h"
#include "Utils.h"
// 
// #ifdef _DEBUG
// #define new DEBUG_NEW
// #undef THIS_FILE
// static char THIS_FILE[] = __FILE__;
// #endif

//---------------------------------------------------------------------------------------
KTimer::KTimer()
:m_bWantToStop(false)
{
}

//---------------------------------------------------------------------------------------
KTimer::~KTimer()
{
}

//---------------------------------------------------------------------------------------
bool KTimer::StartTimer(void)
{
	return StartThread();
}

//---------------------------------------------------------------------------------------
void KTimer::StopTimer(void)
{
	m_bWantToStop = true;
	WaitForStop();

	KAutoLock l(m_secTimerEvent);
	while(m_mapTimerEvent.size())
	{
		TIMER_EVENT_MAP::iterator it = m_mapTimerEvent.begin();
		PTIMER_EVENT pTimerEvent = (PTIMER_EVENT)it->second;
		m_mapTimerEvent.erase(it);
		if(pTimerEvent)
		{
			delete pTimerEvent;
			pTimerEvent=NULL;
		}
	}
}

//---------------------------------------------------------------------------------------
void KTimer::SetTimerEvent(unsigned int nEventID,unsigned long ulElapse)
{
	KAutoLock l(m_secTimerEvent);
	TIMER_EVENT_MAP::iterator it = m_mapTimerEvent.find(nEventID);
	if(it!=m_mapTimerEvent.end())
	{
		PTIMER_EVENT pTimerEvent = (PTIMER_EVENT)it->second;
		if(pTimerEvent)
		{
			pTimerEvent->elapse = ulElapse;
			pTimerEvent->timestamp = GetTimeStamp();
			pTimerEvent->flag = 0;
		}
	}
	else
	{
		PTIMER_EVENT pTimerEvent = new TIMER_EVENT;
		pTimerEvent->elapse = ulElapse;
		pTimerEvent->timestamp = GetTimeStamp();
		pTimerEvent->flag = 0;
		m_mapTimerEvent.insert(TIMER_EVENT_MAP::value_type(nEventID,pTimerEvent));
	}
}

//---------------------------------------------------------------------------------------
void KTimer::KillTimerEvent(unsigned int nEventID)
{
	KAutoLock l(m_secTimerEvent);
	TIMER_EVENT_MAP::iterator it = m_mapTimerEvent.find(nEventID);
	if(it!=m_mapTimerEvent.end())
	{
		PTIMER_EVENT pTimerEvent = (PTIMER_EVENT)it->second;
		if(pTimerEvent)
		{
			pTimerEvent->flag = 1;
		}
	}
}

//---------------------------------------------------------------------------------------
void KTimer::ThreadProcMain(void)
{
	while(!m_bWantToStop)
	{
		ProcessTimerEvent();
		Pause(1);
	}
}

//---------------------------------------------------------------------------------------
bool KTimer::ProcessTimerEvent(void)
{
	KAutoLock l(m_secTimerEvent);
	if(m_mapTimerEvent.size()==0)
	{
		return false;
	}

	for(TIMER_EVENT_MAP::iterator it = m_mapTimerEvent.begin();it!=m_mapTimerEvent.end();)
	{
		unsigned int nEventID = (unsigned int)it->first;
		PTIMER_EVENT pTimerEvent = (PTIMER_EVENT)it->second;
		if(pTimerEvent)
		{
			if(pTimerEvent->flag==1)
			{
				m_mapTimerEvent.erase(it++);
				delete pTimerEvent;
				pTimerEvent=NULL;
			}
			else
			{
				unsigned long ulCurrentTimeStamp = GetTimeStamp();
				unsigned long ulElapse = pTimerEvent->elapse;
				unsigned long ulLastTimeStamp = pTimerEvent->timestamp;

				if(ulCurrentTimeStamp - ulLastTimeStamp > ulElapse)
				{
					pTimerEvent->timestamp = ulCurrentTimeStamp;
					OnTimerEvent(nEventID);
				}
				++it;
			}
		}
	}
	return true;
}
