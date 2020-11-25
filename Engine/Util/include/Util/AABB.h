#pragma once

#include <glm/glm.hpp>

struct AABB
{
    AABB()
        : mMin(0.0f), mMax(0.0f)
    {
    }

    AABB(const glm::vec3& min, const glm::vec3& max)
        : mMin(min), mMax(max)
    {
    }

    glm::vec3 mMin, mMax;
};
