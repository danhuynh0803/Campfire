#include "InterruptFlag.h"

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

template<typename Lockable>
void InterruptFlag::Wait(std::condition_variable_any& conditionVariable, Lockable& lock)
{
	struct customLock
	{
		InterruptFlag* self;
		Lockable& lock;
		customLock(InterruptFlag* self_, std::condition_variable_any& cond, Lockable& lk_) :
			self(self_), lock(lk)
		{
			self->setClearMutex.lock();
			self->threadConditionAny = &cond;
		}
		void Unlock()
		{
			
		}
		void Lock()
		{
			std::lock(self->setClearMutex, lock);
		}
		~customLock()
		{
			self->threadConditionAny = 0;
			self->setClearMutex.unlock();
		}
	};
	customLock cl(this, conditionVariable, lock);
	conditionVariable.wait(cl);
}