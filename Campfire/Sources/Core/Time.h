#ifndef TIME_H
#define TIME_H

#include <chrono>

class Time
{
public:
    static void Start();
    static void Update();
    static double deltaTime;
    static float timeScale;

private:
    static std::chrono::high_resolution_clock::time_point start;
    static std::chrono::high_resolution_clock::time_point last;
};

#endif // TIME_H
