#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include "Particles/Particle.h"

#include <vector>

class ParticleSystem
{
public:

    enum class GenerationPattern
    {
        SEQUENCE,
        BURST
    };

    enum class VelocityPattern
    {
        CONE,
        RADIAL,
        RANDOM
    };

    void GenerateParticles(uint32_t numParticles);
    void OnUpdate(float dt);
    void Draw();

    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 velocity = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 scale = glm::vec3(0.5f);

    glm::vec4 color = glm::vec4(1.0f);
    glm::vec4 colorScaleStart = glm::vec4(0.1f);
    glm::vec4 colorScaleEnd = glm::vec4(1.0f);

    float lifetime = 5.0f;
    int numParticles = 1000;

    GenerationPattern pGeneration = GenerationPattern::SEQUENCE;
    VelocityPattern pVelocity = VelocityPattern::RANDOM;

private:
    std::vector<Particle> particles;
};

#endif // PARTICLE_SYSTEM_H
