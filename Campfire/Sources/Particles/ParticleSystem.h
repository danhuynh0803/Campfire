#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include "Particles/Particle.h"

#include <vector>

class ParticleSystem
{
public:
    void GenerateParticles(uint32_t numParticles);
    void OnUpdate(float dt);
    void Draw();

    glm::vec3 position;
    float lifetime = 50.0f;

private:
    std::vector<Particle> particles;
};

#endif // PARTICLE_SYSTEM_H
