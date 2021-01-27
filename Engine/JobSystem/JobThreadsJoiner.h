#ifndef JOBTHREADSJOINER_H
#define JOBTHREADSJOINER_H

#include <vector>
#include <thread>

class JobThreadsJoiner
{
public:
    explicit JobThreadsJoiner(std::vector<std::jthread>& threads_):
        threads(threads_){}
    ~JobThreadsJoiner()
    {
        for (unsigned long i = 0; i < threads.size(); ++i)
        {
            threads[i].join();
        }
    }

private:
    std::vector<std::jthread>& threads;
};
#endif
