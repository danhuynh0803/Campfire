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

    #define VELOCITY_PATTERN_FIXED 0
    #define VELOCITY_PATTERN_RANDOM 1
    #define VELOCITY_PATTERN_OVER_RADIAL 2

    #define SIZE_PATTERN_FIXED 0
    #define SIZE_PATTERN_RANDOM 1
    #define SIZE_PATTERN_OVER_LIFE_TIME 2

    #define COLOR_PATTERN_FIXED 0
    #define COLOR_PATTERN_RANDOM 1
    #define COLOR_PATTERN_RANDOM_BETWEEN_TWO_COLOR 2
    #define COLOR_PATTERN_OVER_LIFE_TIME 3

    ParticleSystem() = default;
    ~ParticleSystem() = default;
    void Init();
    void GenerateParticles(uint32_t numParticles);
    void GenerateParticle();
    void DebugParticle();
    void OnUpdate(float dt, const glm::vec3& camPosition);
    void OnImGuiRender();
    void Draw(const glm::mat4& transform);

public:
    bool isLooping = true;
    float rateOverTime = 100.0f; // # of particles to spawn per second

    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 velocity = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec2 velocityRandomX = glm::vec2(-1.0f, 1.0f);
    glm::vec2 velocityRandomY = glm::vec2(-1.0f, 1.0f);
    glm::vec2 velocityRandomZ = glm::vec2(-1.0f, 1.0f);
    glm::vec3 acceleration = glm::vec3(0.0f, 0.0f, 0.0f);

    // min and max scale
    glm::vec3 scale = glm::vec3(0.01f, 0.01f,0.01f);
    glm::vec2 scaleRandomX = glm::vec2(0.01f, 0.05f);
    glm::vec2 scaleRandomY = glm::vec2(0.01f, 0.05f);
    glm::vec2 scaleRandomZ = glm::vec2(0.01f, 0.05f);
    glm::vec2 scaleXOverLifeTime = glm::vec2(0.01f, 0.5f);
    glm::vec2 scaleYOverLifeTime = glm::vec2(0.01f, 0.5f);
    glm::vec2 scaleZOverLifeTime = glm::vec2(0.01f, 0.5f);

    glm::vec4 color = glm::vec4(1.0f);
    glm::vec4 colorRandomStart = glm::vec4(1.0f);
    glm::vec4 colorRandomEnd = glm::vec4(1.0f);
    glm::vec4 colorOverLifeTimeStart = glm::vec4(1.0f);
    glm::vec4 colorOverLifeEnd = glm::vec4(1.0f);

    float lifetime = 10.0f;
    int numParticles = 1000;

    bool hasGravity = false;
    float gravity = -9.81f;

    GenerationPattern pGeneration = GenerationPattern::SEQUENCE;
    //VelocityPattern pVelocity = VelocityPattern::RANDOM;

    int pVelocity = VELOCITY_PATTERN_RANDOM;
    int pSize = SIZE_PATTERN_OVER_LIFE_TIME;
    int pColor = COLOR_PATTERN_RANDOM;

private:
    std::vector<Particle> particles;
    SharedPtr<Texture2D> particleTexture;
};

#endif // PARTICLE_SYSTEM_H
