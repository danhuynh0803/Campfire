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

SharedPtr<Texture2D> tex0;
SharedPtr<Texture2D> tex1;
SharedPtr<Texture2D> tex2;
SharedPtr<Texture2D> tex3;
SharedPtr<Texture2D> tex4;

void ParticleSystem::Init()
{
    tex0 = Texture2D::Create("../Assets/Textures/awesomeface.png");
    tex1 = Texture2D::Create("../Assets/Textures/uv.png");
    tex2 = Texture2D::Create("../Assets/Textures/wall.jpg");
    tex3 = Texture2D::Create("../Assets/Textures/container.jpg");
    tex4 = Texture2D::Create("../Assets/Textures/flowmap.png");
}

void ParticleSystem::GenerateParticles(uint32_t numParticles)
{
    for (uint32_t i = 0; i < numParticles; ++i)
    {
        GenerateParticle();
    }
}

void ParticleSystem::GenerateParticle()
{
    Particle particle;
    particle.position = position;
    particle.rotation = glm::vec3(0.0f);
    particle.lifetime = lifetime;

    float t = (float)std::rand()/RAND_MAX;
    particle.color = colorScaleStart*t + colorScaleEnd*(1.0f-t);
    particle.scale = glm::vec3(scale[0]*t + scale[1]*(1.0f - t));

    switch (pVelocity)
    {
        case (VelocityPattern::RADIAL):
        {
            particle.velocity =
                glm::vec3(
                    velocity.x, //* sin( RADIAN((float)i / numParticles * 360) ),
                    velocity.y,
                    velocity.z //* cos( RADIAN((float)i / numParticles * 360) )
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

//GenerateSequence(uint32_t numParticles{)
//{
//}
//
//GenerateBurst(uint32_t numParticles)
//{
//}

void ParticleSystem::OnUpdate(float dt)
{
    float spawnTime = 1.0f / rateOverTime;
    static float timer = spawnTime;
    if (isLooping)
    {
        timer -= dt;
        if (timer <= 0)
        {
            GenerateParticle();
            timer = spawnTime;
        }
    }

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
    transform = glm::scale(transform, glm::vec3(0.1f));

    // Draw a white quad to indication position of PS
    Renderer2D::SubmitQuad(transform, glm::vec4(1.0f));

    for (const auto& particle : particles)
    {
        Renderer2D::DrawBillboard(particle.position, particle.scale, particle.color);

        //Renderer2D::DrawBillboard(particle.position, particle.scale, tex0, particle.color);
    }
}

void ParticleSystem::OnImGuiRender()
{
    ImGui::Begin("Particles");

    ImGui::Separator();

    ImGui::Text("Global Settings");
    ImGui::Checkbox("isLooping", &isLooping);
    ImGui::DragFloat3("System Position", (float*)&position, 0.1f);
    ImGui::InputFloat("Rate over time", &rateOverTime);
    ImGui::InputInt("Number of particles", &numParticles);
    ImGui::InputFloat("Particle Lifetime", &lifetime);
    ImGui::InputFloat("Gravity", &gravity);

    ImGui::Separator();

    ImGui::Text("Particle Settings");
    ImGui::Text("Randomized between the two selected values)");
    ImGui::DragFloat2("min/max Velocity X", (float*)&velocityX, 0.1f);
    ImGui::DragFloat2("min/max Velocity Y", (float*)&velocityY, 0.1f);
    ImGui::DragFloat2("min/max Velocity Z", (float*)&velocityZ, 0.1f);
    ImGui::DragFloat2("min and max scale", (float*)&scale, 0.01f);

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

    if (ImGui::Button("Clear"))
    {
        particles.clear();
    }

    ImGui::End();

}
