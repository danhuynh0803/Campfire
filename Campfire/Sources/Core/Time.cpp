#include "Time.h"

// Public
double Time::deltaTime = 0.0f;
float Time::timeScale = 1.0f;

// Private
std::chrono::high_resolution_clock::time_point Time::start;
std::chrono::high_resolution_clock::time_point Time::last;

void Time::Init()
{
    start = std::chrono::high_resolution_clock::now();
    last  = std::chrono::high_resolution_clock::now();
}

void Time::Update()
{
    start = std::chrono::high_resolution_clock::now();
    deltaTime =
        std::chrono::duration_cast<std::chrono::duration<double>>(start - last).count()
        * timeScale
        ;
    last = start;
}
