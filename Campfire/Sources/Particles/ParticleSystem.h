#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include "Particles/Particle.h"

#include <vector>

class ParticleSystem
{
public:
    // TODO
    enum class GenerationPattern
    {
        SEQUENCE,
        BURST
    };

    // TODO
    enum class VelocityPattern
    {
        CONE,
        RADIAL,
        RANDOM
    };

    void GenerateParticles(uint32_t numParticles);
    void GenerateParticle();
    void OnUpdate(float dt);
    void OnImGuiRender();
    void Draw();

public:
    bool isLooping = true;
    float rateOverTime = 10.0f; // # of particles to spawn per second

    glm::vec3 position = glm::vec3(0.0f);

    glm::vec3 velocity = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::vec2 velocityX = glm::vec2(-1.0f, 1.0f);
    glm::vec2 velocityY = glm::vec2(-1.0f, 1.0f);
    glm::vec2 velocityZ = glm::vec2(-1.0f, 1.0f);

    glm::vec3 scale = glm::vec3(0.1f);

    glm::vec4 color = glm::vec4(1.0f);
    glm::vec4 colorScaleStart = glm::vec4(0.1f);
    glm::vec4 colorScaleEnd = glm::vec4(1.0f);

    float lifetime = 10.0f;
    int numParticles = 1000;

    float gravity = -9.81;

    GenerationPattern pGeneration = GenerationPattern::SEQUENCE;
    VelocityPattern pVelocity = VelocityPattern::RANDOM;

private:
    std::vector<Particle> particles;
};

#endif // PARTICLE_SYSTEM_H
