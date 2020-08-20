#ifndef __TIMER_10_MS_H__
#define __TIMER_10_MS_H__
#include <map>
#include "XAutoLock.h"
#include "XThreadBase.h"
#include "XUtil.h"

class Timer10MsEvent
{
public:
	Timer10MsEvent(void) {};
	virtual ~Timer10MsEvent(void) {};

	virtual void OnTimer10Ms(int64_t curTime64) = 0;
};

class Timer10Ms : public XThreadBase
{
protected:
	Timer10Ms(void);
public:
	static Timer10Ms&Inst();
	virtual ~Timer10Ms(void);

	void RegisterTimer(void* ptr, Timer10MsEvent* pEvent);
	void UnRegisterTimer(void* ptr);

	//* For XThreadBase
	virtual void ThreadProcMain(void);

private:
	bool		b_running_;
	int64_t		last_tick_time64_;

	typedef std::map<void*, Timer10MsEvent*>MapTimer10MsEvent;

	XCritSec	cs_map_timer_;
	MapTimer10MsEvent map_timer_;
};

#endif	// __TIMER_10_MS_H__