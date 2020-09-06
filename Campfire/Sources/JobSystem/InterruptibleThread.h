#ifndef INTERRUPTIBLE_THREAD_H
#define INTERRUPTIBLE_THREAD_H

#include <condition_variable>
#include <atomic>
#include <future>
#include <exception>

struct ThreadInterrupted:public std::exception
{
	const char* what() const throw()
	{
		return "Thread Interrupted Exception";
	}
};
class InterruptFlag
{
public:
    InterruptFlag():threadCondition(0),threadConditionAny(0){}
	void Set();

	bool isSet() const 
	{
		return flag.load(std::memory_order_relaxed);
	}

	void SetConditionVariable(std::condition_variable& conditionVaraible)
	{
		std::lock_guard<std::mutex> lk(setClearMutex);
		threadCondition = &conditionVaraible;
	}

	void ClearConditionVariable()
	{
		std::lock_guard<std::mutex> lk(setClearMutex);
		threadCondition = 0;
	}
	struct ClearConditionVariableDestruct
	{
		~ClearConditionVariableDestruct()
		{
			this_thread_interrupt_flag.ClearConditionVariable();
		}
	};

    template<typename Lockable>
	void Wait(std::condition_variable_any& conditionVariable, Lockable& lockable)
	{
		struct CustomLock
		{
			InterruptFlag* self;
			Lockable& lockable;
			CustomLock(InterruptFlag* self_, std::condition_variable_any& cond, Lockable& lockable_) :
				self(self_), lockable(lockable_)
			{
				self->setClearMutex.lock();
				self->threadConditionAny = &cond;
			}
			void Lock()
			{
				std::lock(self->setClearMutex, lockable);
			}
			
			void Unlock()
			{
				lockable.unlock();
				self->setClearMutex.unlock();
			}

			~CustomLock()
			{
				self->threadConditionAny = 0;
				self->setClearMutex.unlock();
			}
		};
		CustomLock cl(this, conditionVariable, lockable);
		InterruptionPoint();
		conditionVariable.wait(cl);
		InterruptionPoint();
	}
private:
    std::atomic<bool> flag;
    std::condition_variable* threadCondition;
    std::condition_variable_any* threadConditionAny;
    std::mutex setClearMutex;
};

thread_local InterruptFlag this_thread_interrupt_flag;

class InterruptibleThread
{
public:
	template<typename FunctionType>
	InterruptibleThread(FunctionType t)
	{
		std::promise<InterruptFlag*> p;
		internalThread = std::thread([f, &p] {
			p.set_value(&this_thread_interrupt_flag);
			try{ f();} catch(ThreadInterrupted const&){}
		});
		flag = p.get_future().get();
	}
	void Interrupt()
	{
		if (flag)
		{
			flag->Set();
		}
	}
private:
	std::thread internalThread;
	InterruptFlag* flag;
};
void InterruptionPoint()
{
	/*
	Use try{}catch(thread_interrupted&){} block to catch this exception
	*/
	if (this_thread_interrupt_flag.isSet())
	{
		throw ThreadInterrupted();
	}
}

void InterruptibleWait(std::condition_variable& conditonVariable, 
	std::unique_lock<std::mutex>& lock)
{
	InterruptionPoint();
	this_thread_interrupt_flag.SetConditionVariable(conditonVariable);
	InterruptFlag::ClearConditionVariableDestruct guard;
	InterruptionPoint();
	conditonVariable.wait_for(lock, std::chrono::microseconds(1));
	InterruptionPoint();
}
template<typename Predicate>
void InterruptibleWait(std::condition_variable& conditonVariable, 
	std::unique_lock<std::mutex>& lock,
	Predicate pred)
{
	InterruptionPoint();
	this_thread_interrupt_flag.SetConditionVariable(conditonVariable);
	InterruptFlag::ClearConditionVariableDestruct guard;
	while (!this_thread_interrupt_flag.isSet() && !pred())
	{
		conditonVariable.wait_for(lock, std::chrono::microseconds(1))
	}
	InterruptionPoint();
}

template<typename Lockable>
void InterruptibleWait(std::condition_variable_any& conditonVariableAny, Lockable& lockable)
{
	this_thread_interrupt_flag.Wait(conditonVariableAny, lockable);
}

//template<typename T>
//void InterruptibleWait(std::future<T>& uf)
//{
//	while (!this_thread_interrupt_flag.isSet())
//	{
//		if (uf.wait_for(lk,std::chrono::microseconds(1)) == std::future_status::ready)
//			break;
//	}
//	InterruptionPoint();
//}


#endif // INTERRUPTIBLE_THREAD_H