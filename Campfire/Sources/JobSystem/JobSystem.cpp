#include "JobSystem.h"

JobSystem::JobSystem()
    : done(false)
{
}

JobSystem::~JobSystem()
{
    done = false;
}

void JobSystem::Run()
{
    while (!done)
    {
        std::function<void()> fn = jobQueue.front();
        jobQueue.pop();
        if (fn != nullptr)
        {
            fn();
        }
    }
}

void JobSystem::PrepareWorker()
{
    // TODO
}

void JobSystem::Submit(std::function<void()> fn)
{
    jobQueue.push(fn);
}
