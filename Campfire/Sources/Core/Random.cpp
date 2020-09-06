#include "Core/Random.h"

std::random_device Random::rd;
std::mt19937_64 Random::gen64;
std::uniform_int_distribution<uint64_t> Random::dist64;

void Random::Init()
{
    gen64 = std::mt19937_64(rd());
}

uint64_t Random::UINT64T()
{
    return dist64(gen64);
}

float Random::Range(float min, float max)
{
    std::uniform_real_distribution<> dist(min,max);
    return dist(gen64);
}

float Random::Value()
{
    std::uniform_real_distribution<> dist(0.0f, 1.0f);
    return dist(gen64);
}