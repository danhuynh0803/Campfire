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
        glm::vec3 N = cross(E1, E2);

        float det = -glm::dot(mDir, N);
        float invdet = 1.0 / det;
        glm::vec3 AO = mOrig - a;
        glm::vec3 DAO = glm::cross(AO, mDir);

        float u = glm::dot(E2, DAO) * invdet;
        float v = -glm::dot(E1, DAO) * invdet;
        t = glm::dot(AO, N) * invdet;

        return (
           det >= 1e-6
          && t >= 0.0
          && u >= 0.0
          && v >= 0.0
          && (u + v) <= 1.0
        );

        /*
        // TODO: bug in this implementation
        // Intersection using inside-outside test
        // edges
        glm::vec3 e0 = b - a;
        glm::vec3 e1 = c - b;
        glm::vec3 e2 = a - c;

        glm::vec3 n = glm::cross(e0, -e2);

        // Find the distance that lies on triangle's normal
        t = - glm::dot(n, mOrig) / glm::dot(n, mDir);
        if (t < 0) return false; // behind camera

        // point of intersection on the plane
        glm::vec3 p = mOrig + mDir*t;

        // Check that the point is within the triangle using inside-outside test
        // inside the triangle of the point p is to the left of each edge
        glm::vec3 c0 = glm::cross(e0, (p - a));
        glm::vec3 c1 = glm::cross(e1, (p - b));
        glm::vec3 c2 = glm::cross(e2, (p - c));

        return
               glm::dot(c0, n) > 0
            && glm::dot(c1, n) > 0
            && glm::dot(c2, n) > 0
        ;
        */
    }

    glm::vec3 mOrig, mDir;
};
