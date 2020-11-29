#include <math.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/norm.hpp>
#include "Particles/ParticleSystem.h"
#include "Renderer/Renderer2D.h"
#include "Core/FileSystem.h"
#include "Core/ResourceManager.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <random>
#include <algorithm>

#define PI 3.14159265
#define RADIAN(x) (x * PI/180.0f)

void ParticleSystem::Init()
{
    // Default texture
    particleTexture = Texture2D::Create(ASSETS + "/Particles/circle_05.png");
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

    switch (pVelocity)
    {
        case (VELOCITY_PATTERN_RANDOM):
        {
            float randX = ((float)std::rand() / (RAND_MAX));
            float randY = ((float)std::rand() / (RAND_MAX));
            float randZ = ((float)std::rand() / (RAND_MAX));

            particle.velocity =
                glm::vec3(
                    velocityRandomX[0] * randX + velocityRandomX[1] * (1 - randX),
                    velocityRandomY[0] * randY + velocityRandomY[1] * (1 - randY),
                    velocityRandomZ[0] * randZ + velocityRandomZ[1] * (1 - randZ)
                );
            break;
        }
        case (VELOCITY_PATTERN_OVER_RADIAL):
        {
            particle.velocity =
                glm::vec3(
                    velocity.x, //* sin( RADIAN((float)i / numParticles * 360) ),
                    velocity.y,
                    velocity.z //* cos( RADIAN((float)i / numParticles * 360) )
                );
            break;
        }
        default:
        {
            particle.velocity = velocity;
            break;
        }
    }

    switch (pSize)
    {
        case(SIZE_PATTERN_RANDOM):
        {
            float randX = ((float)std::rand() / (RAND_MAX));
            float randY = ((float)std::rand() / (RAND_MAX));
            float randZ = ((float)std::rand() / (RAND_MAX));
            particle.scale = glm::vec3(scaleRandomX[0] * randX + scaleRandomX[1] * (1.0f - randX),
                scaleRandomY[0] * randY + scaleRandomY[1] * (1.0f - randY),
                scaleRandomZ[0] * randZ + scaleRandomZ[1] * (1.0f - randZ));
            break;
        }
        case(SIZE_PATTERN_OVER_LIFE_TIME):
        {
            float randX = ((float)std::rand() / (RAND_MAX));
            float randY = ((float)std::rand() / (RAND_MAX));
            float randZ = ((float)std::rand() / (RAND_MAX));
            particle.scale = glm::vec3(scaleXOverLifeTime[0],
                scaleYOverLifeTime[0],
                scaleZOverLifeTime[0]);
            particle.scaleRate = glm::vec3((scaleXOverLifeTime[1] - scaleXOverLifeTime[0]) / lifetime,
                (scaleYOverLifeTime[1] - scaleYOverLifeTime[0]) / lifetime,
                (scaleZOverLifeTime[1] - scaleZOverLifeTime[0]) / lifetime);
            break;
        }
        default:
        {
            particle.scale = scale;
            break;
        }
    }

    switch (pColor)
    {
        case(COLOR_PATTERN_RANDOM):
        {
            float r = (float)std::rand() / RAND_MAX;
            float g = (float)std::rand() / RAND_MAX;
            float b = (float)std::rand() / RAND_MAX;
            float a = (float)std::rand() / RAND_MAX;
            particle.color = glm::vec4(r, g, b, a);
            break;
        }
        case(COLOR_PATTERN_RANDOM_BETWEEN_TWO_COLOR):
        {
            float t = (float)std::rand() / RAND_MAX;
            particle.color = colorRandomStart * t + colorRandomEnd * (1.0f - t);
            break;
        }
        case(COLOR_PATTERN_OVER_LIFE_TIME):
        {
            particle.color = colorOverLifeTimeStart;
            particle.colorScaleRate = (colorOverLifeEnd - colorOverLifeTimeStart) / lifetime;
            break;
        }
        default:
        {
            particle.color = color;
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
        if (hasGravity)
            particle.velocity += (glm::vec3(0.0f, gravity, 0.0f) + acceleration) * dt;
        else
            particle.velocity += (acceleration * dt);

        if(pSize == SIZE_PATTERN_OVER_LIFE_TIME) particle.scale += particle.scaleRate * dt;
        if(pColor == COLOR_PATTERN_OVER_LIFE_TIME) particle.color += particle.colorScaleRate * dt;
        particle.position += particle.velocity * dt;
        particle.cameraDistance = glm::length2(particle.position - camPosition);

        count++;
    }

    // Sort all particles for transparency ordering
    std::sort(particles.begin(), particles.end());
}

void ParticleSystem::Draw(const glm::mat4& transform)
{
    for (const auto& particle : particles)
    {
        Renderer2D::DrawBillboard(particle.position, particle.scale, particleTexture, particle.color);
    }
}

void ParticleSystem::OnImGuiRender()
{
    ImGui::NewLine();
    if (ImGui::Button("Generate")) { GenerateParticles(numParticles); }
    ImGui::SameLine();
    if (ImGui::Button("Clear")) { particles.clear(); }
    ImGui::Separator();

    ImGui::Text("Global Settings");
    ImGui::Checkbox("isLooping", &isLooping);
    ImGui::DragFloat("Rate over time", &rateOverTime, 1.0f);
    ImGui::DragInt("Number of particles", &numParticles);
    ImGui::DragFloat("Particle Lifetime", &lifetime, 0.1f);
    ImGui::Checkbox("Enable Gravity", &hasGravity);
    ImGui::DragFloat("Gravity", &gravity, 0.01f);

    if (ImGui::ImageButton((ImTextureID)particleTexture->GetRenderID(), ImVec2(64, 64), ImVec2(0,1), ImVec2(1,0), -1, ImVec4(0,0,0,0), ImVec4(0.9, 0.9f, 0.9f, 1.0f)))
    {
        std::string path = FileSystem::OpenFile("*.png");
        if (path.compare("") != 0) // No file selected
        {
            particleTexture = Texture2D::Create(path);
        }
    }
    ImGui::SameLine(); ImGui::Text("Particle Texture");

    ImGui::Separator();

    if (ImGui::TreeNode("Particle Velocity Setting"))
    {
        ImGui::RadioButton("Fixed on Spawn", &pVelocity, VELOCITY_PATTERN_FIXED); ImGui::SameLine();
        ImGui::RadioButton("Randomized on Spawn", &pVelocity, VELOCITY_PATTERN_RANDOM);
        ImGui::DragFloat3("Fixed Velocity", (float*)&velocity, 0.01f);
        ImGui::Text("Randomized Velocity");
        ImGui::DragFloat2("Min/Max Velocity X", (float*)&velocityRandomX, 0.01f);
        ImGui::DragFloat2("Min/Max Velocity Y", (float*)&velocityRandomY, 0.01f);
        ImGui::DragFloat2("Min/Max Velocity Z", (float*)&velocityRandomZ, 0.01f);
        ImGui::TreePop();
    }
    ImGui::Separator();
    if (ImGui::TreeNode("Particle Acceleration Setting"))
    {
        ImGui::DragFloat3("Fixed Acceleration", (float*)&acceleration, 0.01f);
        ImGui::TreePop();
    }
    ImGui::Separator();

    if(ImGui::TreeNode("Particle Size Setting"))
    {
        ImGui::RadioButton("Fixed on Spawn", &pSize, SIZE_PATTERN_FIXED); ImGui::SameLine();
        ImGui::RadioButton("Randomized on Spawn", &pSize, SIZE_PATTERN_RANDOM); ImGui::SameLine();
        ImGui::RadioButton("Size Over Life Time", &pSize, SIZE_PATTERN_OVER_LIFE_TIME);
        ImGui::DragFloat3("Fixed Size", (float*)&scale, 0.01f, 0.0f);
        ImGui::Text("Randomized Size");
        ImGui::DragFloat2("Min/Max Size X", (float*)&scaleRandomX, 0.01f, 0.0f);
        ImGui::DragFloat2("Min/Max Size Y", (float*)&scaleRandomY, 0.01f, 0.0f);
        ImGui::DragFloat2("Min/Max Size Z", (float*)&scaleRandomZ, 0.01f, 0.0f);
        ImGui::Text("Size Over Life Time");
        ImGui::DragFloat2("Start/End Size X", (float*)&scaleXOverLifeTime, 0.01f, 0.0f);
        ImGui::DragFloat2("Start/End Size Y", (float*)&scaleYOverLifeTime, 0.01f, 0.0f);
        ImGui::DragFloat2("Start/End Size Z", (float*)&scaleZOverLifeTime, 0.01f, 0.0f);
        ImGui::TreePop();
    }
    ImGui::Separator();

    if (ImGui::TreeNode("Color Settings"))
    {
        //ImGui::ColorEdit4("Single Color", (float*)&color);
        ImGui::RadioButton("Fixed on Spawn", &pColor, COLOR_PATTERN_FIXED); ImGui::SameLine();
        ImGui::RadioButton("Randomized on Spawn", &pColor, COLOR_PATTERN_RANDOM);
        ImGui::RadioButton("Randomized Between Two Color on Spawn", &pColor, COLOR_PATTERN_RANDOM_BETWEEN_TWO_COLOR);
        ImGui::RadioButton("Color Over Life Time", &pColor, COLOR_PATTERN_OVER_LIFE_TIME);
        ImGui::ColorEdit4("Fixed Color", (float*)&color, 0.01f);
        ImGui::Text("Randomized Between Two Color");
        ImGui::ColorEdit4("Min", (float*)&colorRandomStart, 0.01f);
        ImGui::ColorEdit4("Max", (float*)&colorRandomEnd, 0.01f);
        ImGui::Text("Color Over Life Time");
        ImGui::ColorEdit4("Start Color", (float*)&colorOverLifeTimeStart, 0.01f);
        ImGui::ColorEdit4("End Color", (float*)&colorOverLifeEnd, 0.01f);
        ImGui::TreePop();
    }
    ImGui::Separator();
}
