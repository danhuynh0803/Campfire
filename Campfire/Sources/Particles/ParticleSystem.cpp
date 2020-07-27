#include <math.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
#include "Particles/ParticleSystem.h"
#include "Renderer/Renderer2D.h"
#include "Core/FileSystem.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#define PI 3.14159265
#define RADIAN(x) (x * PI/180.0f)

#include <random>
#include <algorithm>

void ParticleSystem::Init()
{
    // Default texture
    particleTexture = Texture2D::Create("../Assets/Particles/circle_05.png");
}

void ParticleSystem::GenerateParticles(uint32_t numParticles)
{
    // For manual spawning test particles
    //DebugParticle();
    for (uint32_t i = 0; i < numParticles; ++i)
    {
        GenerateParticle();
    }
}

void ParticleSystem::DebugParticle()
{
    for (int i = 0; i < 5; ++i)
    {
        Particle particle;
        particle.scale = glm::vec3(1.0f,1.0f,1.0f);
        particle.position = glm::vec3(0.2f * i, 0.0f, -1.0f + (0.5f * i));
        particle.rotation = glm::vec3(0.0f);
        particle.lifetime = lifetime;
        float t = (float)std::rand() / RAND_MAX;
        particle.color = glm::vec4(0.1f, 0.1f, 0.1f * i, 1.0f);

        particles.push_back(particle);
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

    switch (pScale)
    {
        case(SCALE_Pattern_RANDOM):
        {
            float randX = ((float)std::rand() / (RAND_MAX));
            float randY = ((float)std::rand() / (RAND_MAX));
            float randZ = ((float)std::rand() / (RAND_MAX));
            particle.scale = glm::vec3(scaleX[0] * randX + scaleX[1] * (1.0f - randX),
                scaleY[0] * randY + scaleY[1] * (1.0f - randY),
                scaleZ[0] * randZ + scaleZ[1] * (1.0f - randZ));
            break;
        }
        default:
        {
            particle.scale = scale;
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

void ParticleSystem::OnUpdate(float dt, const glm::vec3& camPosition)
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
        //GenerateParticles((uint32_t)dt*numParticles);
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
        particle.cameraDistance = glm::length2(particle.position - camPosition);

        count++;
    }

    // Sort all particles for transparency ordering
    std::sort(particles.begin(), particles.end());
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
        //Renderer2D::DrawBillboard(particle.position, particle.scale, particle.color);
        Renderer2D::DrawBillboard(particle.position, particle.scale, particleTexture, particle.color);
    }
}

void ParticleSystem::OnImGuiRender()
{
    ImGui::Begin("Particles");

    ImGui::Separator();

    ImGui::Text("Global Settings");
    ImGui::Checkbox("isLooping", &isLooping);
    ImGui::DragFloat3("System Position", (float*)&position, 0.1f);
    ImGui::DragFloat("Rate over time", &rateOverTime, 1.0f);
    ImGui::DragInt("Number of particles", &numParticles);
    ImGui::DragFloat("Particle Lifetime", &lifetime, 0.1f);
    ImGui::DragFloat("Gravity", &gravity, 0.1f);

    ImGui::Separator();

    ImGui::Text("Particle Settings");
    ImGui::Text("Randomized between the two selected values)");
    ImGui::DragFloat2("min/max Velocity X", (float*)&velocityX, 0.1f);
    ImGui::DragFloat2("min/max Velocity Y", (float*)&velocityY, 0.1f);
    ImGui::DragFloat2("min/max Velocity Z", (float*)&velocityZ, 0.1f);
    ImGui::Separator();

    ImGui::Text("Particle Scale Modes)");
    ImGui::RadioButton("Fixed", &pScale, 0); ImGui::SameLine();
    ImGui::RadioButton("Randomized", &pScale, 1);
    ImGui::DragFloat3("Fixed Particle Scale", (float*)&scale, 0.1f,0.0f);
    ImGui::Text("Randomized between the two selected values)"); 
    ImGui::DragFloat2("min and max scale X", (float*)&scaleX, 0.01f, 0.0f);
    ImGui::DragFloat2("min and max scale Y", (float*)&scaleY, 0.01f, 0.0f);
    ImGui::DragFloat2("min and max scale Z", (float*)&scaleZ, 0.01f, 0.0f);
    ImGui::Separator();

    ImGui::Text("Color Settings");
    //ImGui::ColorEdit4("Single Color", (float*)&color);
    ImGui::ColorEdit4("Start", (float*)&colorScaleStart);
    ImGui::ColorEdit4("End", (float*)&colorScaleEnd);

    ImGui::Separator();

    if (ImGui::ImageButton((ImTextureID)particleTexture->GetRenderID(), ImVec2(64, 64), ImVec2(0,1), ImVec2(1,0), -1, ImVec4(0,0,0,0), ImVec4(0.9, 0.9f, 0.9f, 1.0f)))
    {
        std::string path = FileSystem::OpenFile("*.png");
        if (path.compare("") != 0) // No file selected
        {
            particleTexture = Texture2D::Create(path);
        }
    }

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
