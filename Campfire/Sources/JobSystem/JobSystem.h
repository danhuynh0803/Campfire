#ifndef JOB_SYSTEM_H
#define JOB_SYSTEM_H

#include <atomic>
#include <vector>
#include <future>

#include "ThreadSafeQueue.h"
#include "JobStealingQueue.h"
#include "Job.h"
#include "JobThreadsJoiner.h"

class JobSystem
{
public:
    JobSystem();
    ~JobSystem();

    void Init();
    void JobSystem::JobThread(unsigned index);
    void Run();
    void Wait();
    bool HasWork();
    bool JobSystem::PopJobFromLocalQueue(Job& job);
    bool PopJobFromPoolQueue(Job& job);
    bool JobSystem::PopJobOtherThreadQueue(Job& job);

    template <typename FunctionType>
    std::future<typename std::result_of<FunctionType()>::type>
        Submit(FunctionType f)
    {
        typedef typename std::result_of<FunctionType()>::type result_type;
        std::packaged_task<result_type()> task(std::move(f));
        std::future<result_type> res(task.get_future());
        jobPoolQueue.Push(std::move(task));
        return res;
    }

private:
    std::atomic_bool done;
    ThreadSafeQueue<Job> jobPoolQueue;
    std::vector<std::unique_ptr<JobStealingQueue<Job>>> jobStealingQueues;
    std::vector<std::thread> threadPool;
    JobThreadsJoiner joiner;
    inline static thread_local JobStealingQueue<Job>* localJobQueue;
    inline static thread_local unsigned myIndex;
};

#endif // JOB_SYSTEM_H
