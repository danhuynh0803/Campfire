#ifndef PARTICLE_H
#define PARTICLE_H

#include <glm/glm.hpp>

struct Particle
{
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    glm::vec3 velocity;

    glm::vec4 color;

    float lifetime;
};

#endif // PARTICLE_H
