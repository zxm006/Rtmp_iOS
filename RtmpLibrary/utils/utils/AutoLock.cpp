#include "stdafx.h"
#include "AutoLock.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------------------
KAutoLock::KAutoLock(KCritSec &rCritSec)
:m_rCritSec(rCritSec)
{
	m_rCritSec.Lock();
}

//---------------------------------------------------------------------------------------
KAutoLock::~KAutoLock()
{
	m_rCritSec.Unlock();
}

