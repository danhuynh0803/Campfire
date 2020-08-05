#ifndef RANDOM_H
#define RANDOM_H

#include <random>

class Random
{
public:
    static void Init();

    // Returns a random uint64_t
    static uint64_t UINT64T();

    // Returns a random float between [min, max]
    static float Range(float min, float max);

    // Returns a random number between [0, 1]
    static float Value();

private:
    static std::random_device rd;
    static std::mt19937_64 gen64;
    static std::uniform_int_distribution<uint64_t> dist64;
};

#endif // RANDOM_H
