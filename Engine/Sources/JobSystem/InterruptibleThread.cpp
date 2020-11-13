#include "InterruptibleThread.h"

void InterruptFlag::Set()
{
	//the looset memory order
	flag.store(true, std::memory_order_relaxed);
	std::lock_guard<std::mutex> lock(setClearMutex);
	if (threadCondition)
	{
		threadCondition->notify_all();
	}
	else if (threadConditionAny)
	{
		threadConditionAny->notify_all();
	}
}