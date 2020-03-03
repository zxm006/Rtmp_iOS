/* KAutoLock.h */

#pragma once

#include "CritSec.h"

//---------------------------------------------------------------------------------------
class KAutoLock  
{
public:
	KAutoLock(KCritSec &rCritSec);
	~KAutoLock();

protected:
    KCritSec& m_rCritSec;
};

