#include <math.h>
#include <glm/gtc/matrix_transform.hpp>
#include "Particles/ParticleSystem.h"
#include "Renderer/Renderer2D.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#define PI 3.14159265
#define RADIAN(x) (x * PI/180.0f)

#include <random>

void ParticleSystem::GenerateParticles(uint32_t numParticles)
{
    for (uint32_t i = 0; i < numParticles; ++i)
    {
        Particle particle;
        particle.position = position;
        particle.rotation = glm::vec3(0.0f);
        particle.scale = scale;
        particle.lifetime = lifetime;

        float t = (float)std::rand()/RAND_MAX;
        particle.color = colorScaleStart*t + colorScaleEnd*(1.0f-t);

        switch (pVelocity)
        {
            case (VelocityPattern::RADIAL):
            {
                particle.velocity =
                    glm::vec3(
                        velocity.x * sin( RADIAN((float)i / numParticles * 360) ),
                        velocity.y,
                        velocity.z * cos( RADIAN((float)i / numParticles * 360) )
                    );
                break;
            }
            case (VelocityPattern::RANDOM):
            {
                float randX = ((float) std::rand() / (RAND_MAX));
                float randY = ((float) std::rand() / (RAND_MAX));
                float randZ = ((float) std::rand() / (RAND_MAX));

                particle.velocity =
                    glm::vec3(
                        velocityX[0]*randX + velocityX[1]*(1-randX),
                        velocityY[0]*randY + velocityY[1]*(1-randY),
                        velocityZ[0]*randZ + velocityZ[1]*(1-randZ)
                    );
                break;
            }
            default:
            {
                particle.velocity = velocity;
                break;
            }
        }

        particles.push_back(particle);
    }
}

//GenerateSequence(uint32_t numParticles{)
//{
//}
//
//GenerateBurst(uint32_t numParticles)
//{
//}

void ParticleSystem::OnUpdate(float dt)
{
    // Update each particle position
    size_t count = 0;
    for (auto& particle : particles)
    {
        particle.lifetime -= dt;

        if (particle.lifetime < 0)
        {
            particles.erase(particles.begin()+count);
            continue;
        }

        particle.position.x += particle.velocity.x * dt;
        particle.velocity.y += 0.5f * gravity * dt*dt;
        particle.position.y += particle.velocity.y * dt;
        particle.position.z += particle.velocity.z * dt;

        count++;
    }
}


void ParticleSystem::Draw()
{
    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, position);

    // Draw a red quad to indication position of PS
    Renderer2D::SubmitQuad(transform, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

    for (const auto& particle : particles)
    {
        transform = glm::mat4(1.0f);
        transform = glm::translate(transform, particle.position);
        transform = glm::scale(transform, particle.scale);
        Renderer2D::SubmitQuad(transform, particle.color);
    }
}

void ParticleSystem::OnImGuiRender()
{
    ImGui::Begin("Particles");
    ImGui::Text("Global Settings");
    ImGui::InputInt("Number of particles", &numParticles);
    ImGui::InputFloat("Particle Lifetime", &lifetime);
    ImGui::InputFloat("Gravity", &gravity);

    ImGui::Separator();

    ImGui::Text("Positional Settings");
    ImGui::SliderFloat3("Position", (float*)&position, -10.0f, 10.0f);
    //ImGui::SliderFloat3("Velocity", (float*)&velocity, -10.0f, 10.0f);
    ImGui::SliderFloat("minX", (float*)&velocityX[0], 0.0f,-10.0f);
    ImGui::SliderFloat("maxX", (float*)&velocityX[1], 0.0f, 10.0f);
    ImGui::SliderFloat("minY", (float*)&velocityY[0], 0.0f,-10.0f);
    ImGui::SliderFloat("maxY", (float*)&velocityY[1], 0.0f, 10.0f);
    ImGui::SliderFloat("minZ", (float*)&velocityZ[0], 0.0f,-10.0f);
    ImGui::SliderFloat("maxZ", (float*)&velocityZ[1], 0.0f, 10.0f);
    ImGui::SliderFloat3("Scale", (float*)&scale, 0.0f, 1.0f);

    ImGui::Separator();

    ImGui::Text("Color Settings");
    //ImGui::ColorEdit4("Single Color", (float*)&color);
    ImGui::ColorEdit4("Start", (float*)&colorScaleStart);
    ImGui::ColorEdit4("End", (float*)&colorScaleEnd);

    ImGui::Separator();

    if (ImGui::Button("Generate"))
    {
        GenerateParticles(numParticles);
    }

    ImGui::End();

}
