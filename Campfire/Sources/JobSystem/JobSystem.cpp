#include "JobSystem.h"

JobSystem::JobSystem()
    : done(false)
{
}

JobSystem::~JobSystem()
{
    done = true;
}

void JobSystem::Init()
{
    auto numThreads = std::thread::hardware_concurrency();

    try
    {
        for (int i = 0; i < numThreads; ++i)
        {
            threadPool.push_back(std::thread(&JobSystem::Run, this));
        }
    }
    catch (...)
    {
        done = true;
        throw;
    }
}

bool JobSystem::HasWork()
{
    // TODO: maybe not needed
//    Job job;
//    if (jobQueue.pop_front(job))
//    {
//        return true;
//    }
    return false;
}

void JobSystem::Run()
{
    while (!done)
    {
        Job job;
        if (jobQueue.TryPop(job))
        {
            //job.fn(job.args);
            job();
        }
        else
        {
            std::this_thread::yield();
        }
    }
}

//template <typename FunctionType>
//std::future<typename std::result_of<FunctionType()>::type>
//    JobSystem::Submit(FunctionType f)
//{
//
//}

void JobSystem::Wait()
{
    // Wait until all tasks are complete
    //while (!jobQueue.Empty()) {}
    //for (int i = 0; i < threadPool.size(); ++i)
    //{
    //    if (threadPool[i].joinable)
    //        threadPool[i].join;
    //}
}
