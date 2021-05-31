#pragma once

#include <algorithm>
#include <glm/glm.hpp>
#include <glm/common.hpp>

struct Ray
{
    Ray(const glm::vec3& orig, const glm::vec3& dir)
        : mOrig(orig), mDir(dir)
    {
    }

    bool IntersectAABB(const AABB& aabb, float& t)
    {
        const glm::vec3& min = aabb.mMin;
        const glm::vec3& max = aabb.mMax;

        glm::vec3 invDir;
        invDir.x = 1.0f / mDir.x;
        invDir.y = 1.0f / mDir.y;
        invDir.z = 1.0f / mDir.z;

        float txMin = (min.x - mOrig.x) * invDir.x;
        float txMax = (max.x - mOrig.x) * invDir.x;

        float tyMin = (min.y - mOrig.y) * invDir.y;
        float tyMax = (max.y - mOrig.y) * invDir.y;

        float tzMin = (min.z - mOrig.z) * invDir.z;
        float tzMax = (max.z - mOrig.z) * invDir.z;

        float tmin =
            glm::max(
                glm::max(
                    glm::min(txMin, txMax),
                    glm::min(tyMin, tyMax)
                ),
                glm::min(tzMin, tzMax)
            );

        float tmax =
            glm::min(
                glm::min(
                    glm::max(txMin, txMax),
                    glm::max(tyMin, tyMax)
                ),
                glm::max(tzMin, tzMax)
            );

        // Intersecting an object but its behind the camera
        if (tmax < 0)
        {
            t = tmax;
            return false;
        }

        // No intersections
        if (tmin > tmax)
        {
            t = tmax;
            return false;
        }

        // Else, theres an intersection at tmin
        t = tmin;
        return true;
    }

    bool IntersectTriangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, float& t)
    {
        // Intersection using Moller-Trumbore
        glm::vec3 E1 = b - a;
        glm::vec3 E2 = c - a;
        glm::vec3 P = glm::cross(mDir, E2);

        float epsilon = 1e-6;
        // Det = (D x E2) dot E1
        float det = glm::dot(P, E1);
        if (det > -epsilon && det < epsilon)
            return false;

        float invDet = 1.0 / det;

        // Finding u,v,t with Cramer's rule
        glm::vec3 T = mOrig - a;
        float u = glm::dot(T, P) * invDet;
        if (u < 0 || u > 1)
            return false;

        glm::vec3 Q = glm::cross(T, E1);
        float v = glm::dot(mDir, Q) * invDet;
        if (v < 0 || u + v > 1)
            return false;

        t = glm::dot(E2, Q) * invDet;
        // Check if point is behind camera
        if (t < 0)
            return false;

        return true;
    }

    glm::vec3 mOrig, mDir;
};
