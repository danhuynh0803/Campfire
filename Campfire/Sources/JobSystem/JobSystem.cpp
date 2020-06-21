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

    for (int i = 0; i < numThreads; ++i)
    {
        std::thread worker([this] {this->Run(); });        
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

void JobSystem::Wait()
{
//    for (int i = 0; i < threadPool.size(); ++i)
//    {
//        if (threadPool[i].joinable)
//            threadPool[i].join;
//    }
}
