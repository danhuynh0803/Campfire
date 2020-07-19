#include <math.h>
#include <glm/gtc/matrix_transform.hpp>
#include "Particles/ParticleSystem.h"
#include "Renderer/Renderer2D.h"

#define PI 3.14159265
#define RADIAN(x) (x * PI/180.0f)

void ParticleSystem::GenerateParticles(uint32_t numParticles)
{
    float power = 0.1f;
    for (uint32_t i = 0; i < numParticles; ++i)
    {
        Particle particle =
        {
            position,
            glm::vec3(0.0f),
            glm::vec3(0.025f),
            glm::vec3(
                    power * sin( RADIAN((float)i / numParticles * 360) ),
                    3.0f,
                    power * cos( RADIAN((float)i / numParticles * 360) )
            ),
            glm::vec4((float)i / numParticles, (float)i / numParticles, 1.0f, 1.0f),
            lifetime
        };

        particles.push_back(particle);
    }
}

void ParticleSystem::OnUpdate(float dt)
{
    // Update each particle position
    float gravity = -9.81f;
    for (auto& particle : particles)
    {
        particle.position.x += particle.velocity.x * dt;
        particle.velocity.y += 0.5f * gravity * dt*dt;
        particle.position.y += particle.velocity.y * dt;
        particle.position.z += particle.velocity.z * dt;

        //particle.lifetime -= dt;
    }
}


void ParticleSystem::Draw()
{
    for (const auto& particle : particles)
    {
        if (particle.lifetime > 0)
        {
            glm::mat4 transform = glm::mat4(1.0f);
            transform = glm::translate(transform, particle.position);
            transform = glm::scale(transform, particle.scale);
            Renderer2D::SubmitQuad(transform, particle.color);
        }
    }
}
