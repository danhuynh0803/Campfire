#pragma once

#include "Vulkan/VulkanContext.h"
#include "Scene/Camera.h"
#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include "Scene/Component.h"
#include "Renderer/FrameGraph.h"

namespace global
{
//========================================================
struct CameraUBO
{
    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 viewProj;
    glm::vec4 pos;
};

//========================================================
struct Light
{
    glm::vec4 pos;
    glm::vec4 color;
    glm::vec3 dir;
    float intensity;
};

struct RenderContext
{
    CameraUBO cameraUBO;
    static const int maxNumLights = 100;
    std::array<Light, maxNumLights> lights;

    std::vector<SharedPtr<VulkanUniformBuffer>> mCameraUBOs;
    std::vector<SharedPtr<VulkanUniformBuffer>> mLightUBOs;
    std::vector<vk::UniqueDescriptorSet> mDescriptorSets;

    void Init(const SharedPtr<cf::Pipeline>& pipeline)
    {
        auto device = VulkanContext::Get()->GetDevice()->GetVulkanDevice();
        auto swapChainSize = VulkanContext::Get()->GetSwapChain()->GetImages().size();

        // Create global descriptorSet (Should be at set 0)
        // TODO use enums instead of hardcoded ints later on
        auto allocInfo = vk::initializers::DescriptorSetAllocateInfo(
            VulkanContext::Get()->GetDescriptorPool(),
            1,
            &pipeline->mDescriptorSetLayouts.at(0).get()
        );

        mDescriptorSets = device.allocateDescriptorSetsUnique(allocInfo);

        for (size_t i = 0; i < swapChainSize; ++i)
        {
            { // Create camera UBOs
                mCameraUBOs.emplace_back(
                    CreateSharedPtr<VulkanUniformBuffer>(
                        sizeof(CameraUBO)
                    )
                );

                BufferLayout cameraLayout =
                {
                    { ShaderDataType::MAT4,   "view" },
                    { ShaderDataType::MAT4,   "proj" },
                    { ShaderDataType::MAT4,   "viewProj" },
                    { ShaderDataType::FLOAT4, "pos" },
                };

                mCameraUBOs[i]->UpdateDescriptorSet(
                    mDescriptorSets.at(0),
                    cameraLayout, 0
                );
            }

            // TODO switch to storage buffer
            { // Create Light UBOs
                mLightUBOs.emplace_back(
                    CreateSharedPtr<VulkanUniformBuffer>(
                        sizeof(lights) + sizeof(glm::vec4)
                    )
                );

                BufferLayout lightLayout =
                {
                    { ShaderDataType::FLOAT4, "pos"       },
                    { ShaderDataType::FLOAT4, "color"     },
                    { ShaderDataType::FLOAT3, "dir"       },
                    { ShaderDataType::FLOAT , "intensity" },
                };

                mLightUBOs[i]->UpdateDescriptorSet(
                    mDescriptorSets.at(0),
                    1, maxNumLights*lightLayout.GetStride() + sizeof(glm::vec4)
                );
            }
        }
    }

    void Update(
        const SharedPtr<Camera> & camera
      , const SharedPtr<Scene> & scene
      , int frameIdx
    )
    {
        // Update camera ubo
        cameraUBO.view = camera->GetViewMatrix();
        cameraUBO.proj = camera->GetProjMatrix();
        cameraUBO.proj[1][1] *= -1;
        cameraUBO.viewProj = cameraUBO.proj * cameraUBO.view;
        cameraUBO.pos = glm::vec4(camera->pos, 1.0f);
        mCameraUBOs[frameIdx]->SetData(&cameraUBO, 0, sizeof(CameraUBO));

        // Update light ubo
        uint32_t numLights = 0;
        auto group = scene->registry.group<LightComponent>(entt::get<TransformComponent>);
        for (auto entity : group)
        {
            auto [transformComponent, lightComponent] = group.get<TransformComponent, LightComponent>(entity);

            Light light;
            light.pos = glm::vec4(transformComponent.position, 0.0f);
            light.dir = glm::vec3(0.0f);
            light.intensity = lightComponent.intensity;
            light.color = lightComponent.color;
            lights[numLights++] = light;
        }
        mLightUBOs[frameIdx]->SetData(lights.data(), 0, sizeof(lights));
        mLightUBOs[frameIdx]->SetData(&numLights, sizeof(lights), sizeof(uint32_t));
    }
};

enum MatType
{
    DIFF = 0,
    SPEC = 1,
    REFR = 2,
};

struct Sphere
{
    glm::vec3 pos;
    float radius;
    glm::vec3 emission;
    float pad0;
    glm::vec3 albedo;
    float pad1;
    int mat;
    int id;
    glm::vec2 pad2;
};

//========================================================
struct Quad
{
    glm::vec3 a,b,c,d;
    float pad0;
    glm::vec3 emission;
    float pad1;
    glm::vec3 albedo;
    float pad2;
    int mat;
    int id;
    glm::vec2 padding;
};

struct RayTraceScene
{
    SharedPtr<VulkanBuffer> sphereSSBO;
    SharedPtr<VulkanBuffer> quadSSBO;
    std::vector<Sphere> spheres;
    std::vector<Quad> quads;
    std::vector<vk::UniqueDescriptorSet> mDescriptorSets;

    vk::Device mDevice;

    void Init(const SharedPtr<cf::Pipeline>& pipeline)
    {
        mDevice = VulkanContext::Get()->GetDevice()->GetVulkanDevice();

        int maxRow = 1;
        int maxCol = 5;
        int id = 0;
        for (int i = 0; i < maxRow; ++i)
        {
            for (int j = 0; j < maxCol; ++j)
            {
                Sphere sphere{};
                sphere.radius = 0.5f;
                sphere.pos = glm::vec3(j+sphere.radius, i, 0);
                sphere.emission = glm::vec3(1000.0f);
                sphere.albedo = glm::vec3((float)j / maxCol, (float)i / maxRow, 1.0f);
                sphere.mat = MatType::DIFF;
                sphere.id = id++;

                spheres.emplace_back(sphere);
            }
        }

        Quad light {};
        light.a = glm::vec3(0);
        light.b = glm::vec3(0);
        light.c = glm::vec3(0);
        light.d = glm::vec3(0);
        light.emission = glm::vec3(10000.0f);
        light.albedo = glm::vec3(1.0f);
        light.mat = MatType::DIFF;
        light.id = 0;
        quads.emplace_back(light);

        vk::DescriptorSetAllocateInfo info {};
        info.descriptorPool = VulkanContext::Get()->GetDescriptorPool();
        info.descriptorSetCount = 1;
        // Use layout from set 2 of compute pipeline, which is for scene data
        info.pSetLayouts = &pipeline->mDescriptorSetLayouts.at(2).get();

        mDescriptorSets = mDevice.allocateDescriptorSetsUnique(info);

        { // -- sphere SSBO
            // TODO: No need to flush since coherent, but investigate
            // what the performance hit is since we're not using
            // device local memory
            sphereSSBO = CreateSharedPtr<VulkanBuffer>(
                vk::BufferUsageFlagBits::eStorageBuffer,
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
                spheres.size() * sizeof(Sphere),
                vk::SharingMode::eExclusive
            );

            void* data = sphereSSBO->Map();
            memcpy(data, spheres.data(), spheres.size() * sizeof(Sphere));

            // Update DescriptorSet
            vk::DescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = sphereSSBO->mBuffer.get();
            bufferInfo.offset = 0;
            bufferInfo.range = sphereSSBO->mSize;

            vk::WriteDescriptorSet writeInfo{};
            writeInfo.dstSet = mDescriptorSets.at(0).get();
            writeInfo.dstBinding = 0;
            writeInfo.dstArrayElement = 0;
            writeInfo.descriptorType = vk::DescriptorType::eStorageBuffer;
            writeInfo.descriptorCount = 1;
            writeInfo.pBufferInfo = &bufferInfo;

            mDevice.updateDescriptorSets(1, &writeInfo, 0, nullptr);
        }

        { // -- quad SSBO
            quadSSBO = CreateSharedPtr<VulkanBuffer>(
                vk::BufferUsageFlagBits::eStorageBuffer,
                vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
                quads.size() * sizeof(Quad),
                vk::SharingMode::eExclusive
            );

            void* data = quadSSBO->Map();
            memcpy(data, quads.data(), quads.size() * sizeof(Quad));

            // Update DescriptorSet
            vk::DescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = quadSSBO->mBuffer.get();
            bufferInfo.offset = 0;
            bufferInfo.range = quadSSBO->mSize;

            vk::WriteDescriptorSet writeInfo{};
            writeInfo.dstSet = mDescriptorSets.at(0).get();
            writeInfo.dstBinding = 1;
            writeInfo.dstArrayElement = 0;
            writeInfo.descriptorType = vk::DescriptorType::eStorageBuffer;
            writeInfo.descriptorCount = 1;
            writeInfo.pBufferInfo = &bufferInfo;

            mDevice.updateDescriptorSets(1, &writeInfo, 0, nullptr);
        }
    }

    void Update(float dt)
    {
    }

    //// TODO add image barrier from compute to fragment
};

} // namespace global
