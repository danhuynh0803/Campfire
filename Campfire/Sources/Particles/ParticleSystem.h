#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include "Particles/Particle.h"

class ParticleSystem
{
public:
    GenerateParticles(uint32_t numParticles);
    OnUpdate(float dt);
    Draw();

private:
};

#endif // PARTICLE_SYSTEM_H
