#ifndef JOB_SYSTEM_H
#define JOB_SYSTEM_H

#include <atomic>
#include <vector>
#include <future>

#include "ThreadSafeQueue.h"
#include "Job.h"

class JobSystem
{
public:
    JobSystem();
    ~JobSystem();

    void Init();
    void Run();
    void Wait();
    bool HasWork();

    template <typename FunctionType>
    std::future<typename std::result_of<FunctionType()>::type>
        Submit(FunctionType f)
    {
        typedef typename std::result_of<FunctionType()>::type result_type;
        std::packaged_task<result_type()> task(std::move(f));
        std::future<result_type> res(task.get_future());
        jobQueue.push(std::move(task));
        return res;
    }

private:
    std::atomic_bool done;
    ThreadSafeQueue<Job> jobQueue;
};

#endif // JOB_SYSTEM_H
