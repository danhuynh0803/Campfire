#ifndef PARTICLE_H
#define PARTICLE_H

#include <glm/glm.hpp>

struct Particle
{
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    glm::vec3 scaleRate;

    glm::vec4 color;
    glm::vec4 colorScaleRate;

    float lifetime;

    float cameraDistance;

    bool operator < (Particle& other)
    {
        return this->cameraDistance > other.cameraDistance;
    }
};

#endif // PARTICLE_H
