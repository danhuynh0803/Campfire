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
    // TODO
    return 0.0f;
}

float Random::Value()
{
    // TODO
    return 0.0f;
}
