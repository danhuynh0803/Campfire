#ifndef TIMER_H
#define TIMER_H

#include <chrono>
#include <string>

#include "Log.h"

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
        GetTime();
    }

    void Reset()
    {
        start = std::chrono::high_resolution_clock::now();
    }

    void GetTime()
    {
        end = std::chrono::high_resolution_clock::now();
        duration = end - start;
        double ms = duration.count() * 1000.0f;

        LOG_TRACE("Timer: {0}: {1}ms", msg, ms);
    }
};

#endif // TIMER_H
