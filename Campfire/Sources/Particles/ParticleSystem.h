#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include "Core/Base.h"
#include "Renderer/Texture.h"
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

    ParticleSystem() = default;
    ~ParticleSystem() = default;
    void Init();
    void GenerateParticles(uint32_t numParticles);
    void GenerateParticle();
    void DebugParticle();
    void OnUpdate(float dt, const glm::vec3& camPosition);
    void OnImGuiRender();
    void Draw();

public:
    bool isLooping = true;
    float rateOverTime = 100.0f; // # of particles to spawn per second

    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 velocity = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec2 velocityX = glm::vec2(-1.0f, 1.0f);
    glm::vec2 velocityY = glm::vec2(-1.0f, 1.0f);
    glm::vec2 velocityZ = glm::vec2(-1.0f, 1.0f);

    // min and max scale
    glm::vec2 scale = glm::vec2(0.05f, 0.1f);

    glm::vec4 color = glm::vec4(1.0f);
    glm::vec4 colorScaleStart = glm::vec4(1.0f);
    glm::vec4 colorScaleEnd = glm::vec4(1.0, 0.0f, 0.0f, 1.0f);

    float lifetime = 10.0f;
    int numParticles = 1000;

    float gravity = -9.81;

    GenerationPattern pGeneration = GenerationPattern::SEQUENCE;
    VelocityPattern pVelocity = VelocityPattern::RANDOM;

private:
    std::vector<Particle> particles;
    SharedPtr<Texture2D> particleTexture;
};

#endif // PARTICLE_SYSTEM_H
