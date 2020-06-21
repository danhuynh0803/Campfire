#ifndef JOB_SYSTEM_H
#define JOB_SYSTEM_H

#include <functional>
#include <queue>
#include <thread>
#include <atomic>

#include "Job.h"

class JobSystem
{
public:
    JobSystem();
    ~JobSystem();

    void Run();
    void PrepareWorker(); // TODO
    void Submit(std::function<void()> fn);

private:
    std::atomic_bool done;
    std::queue<std::function<void()>> jobQueue;

};

#endif // JOB_SYSTEM_H
