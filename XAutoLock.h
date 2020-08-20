// XAutoLock.h: interface for the AutoLock class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __XAUTOLOCK_H__
#define __XAUTOLOCK_H__

#include "XCritSec.h"

class XAutoLock  
{
public:
	XAutoLock(XCritSec&rXCritSec);
	~XAutoLock(void);
protected:
    XCritSec& m_rXCritSec;
};

#endif 