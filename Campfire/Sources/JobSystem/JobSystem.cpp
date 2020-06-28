#include "JobSystem.h"
#include "JobThreadsJoiner.h"

JobSystem::JobSystem()
    : done(false), joiner(threadPool)
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
            jobStealingQueues.push_back(std::unique_ptr<JobStealingQueue<Job>>(new JobStealingQueue<Job>));
        }
        for (int i = 0; i < numThreads; ++i)
        {
            threadPool.push_back(std::thread(&JobSystem::JobThread, this,i));
        }
    }
    catch (...)
    {
        done = true;
        throw;
    }
}

void JobSystem::JobThread(unsigned index) {
    myIndex = index;
    localJobQueue = jobStealingQueues[myIndex].get();
    while (!done) { return Run(); }
}

void JobSystem::Run()
{
    while (!done)
    {
        Job job;
        if (PopJobFromLocalQueue(job)||
            PopJobFromPoolQueue(job)||
            PopJobOtherThreadQueue(job))
        {
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
    while (!jobPoolQueue.Empty()) {}
}

bool JobSystem::PopJobFromLocalQueue(Job& job) {
    return localJobQueue && localJobQueue->TryPop(job);
}

bool JobSystem::PopJobFromPoolQueue(Job& job) {
    return jobPoolQueue.TryPop(job);
}

bool JobSystem::PopJobOtherThreadQueue(Job& job) {
    for (unsigned i = 0; i < jobStealingQueues.size(); ++i)
    {
        unsigned const index = (myIndex + i + 1) % jobStealingQueues.size();
        if (jobStealingQueues[index]->TrySteal(job))
        {
            return true;
        }
    }
    return false;
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