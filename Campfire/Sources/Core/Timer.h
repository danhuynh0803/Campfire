#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <string>

struct Timer
{
    std::chrono::high_resolution_clock::time_point start, end;
    std::chrono::duration<float> duration;
    std::string msg;

    Timer(std::string msg)
    {
        this->msg = msg;
        start = std::chrono::high_resolution_clock::now();
    }

    ~Timer()
    {
        LOG_TRACE("Timer: {0}: {1}ms", msg, GetTime());
    }

    void Reset()
    {
        start = std::chrono::high_resolution_clock::now();
    }

    double GetTime()
    {
        end = std::chrono::high_resolution_clock::now();
        duration = end - start;
        // convert to ms
        return (duration.count() * 1000.0);
    }
};

#endif // TIMER_H
