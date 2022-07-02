#pragma once

#include "Vulkan/VulkanContext.h"
#include "Scene/Camera.h"
#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include "Scene/Component.h"

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

struct GlobalInfo
{
    CameraUBO cameraUBO;
    static const int maxNumLights = 100;
    std::array<Light, maxNumLights> lights;

    std::vector<SharedPtr<VulkanUniformBuffer>> mCameraUBOs;
    std::vector<SharedPtr<VulkanUniformBuffer>> mLightUBOs;
    std::vector<vk::UniqueDescriptorSet> mDescriptorSets;

    void Init()
    {
        auto device = VulkanContext::Get()->GetDevice()->GetVulkanDevice();
        auto graphicsPipeline = VulkanContext::Get()->mFrameGraph->GetGraphicsPipeline("models");
        auto swapChainSize = VulkanContext::Get()->GetSwapChain()->GetImages().size();

        // Create global descriptorSet (Should be at set 0)
        // TODO use enums instead of hardcoded ints later on
        auto allocInfo = vk::initializers::DescriptorSetAllocateInfo(
            VulkanContext::Get()->GetDescriptorPool(),
            1,
            &graphicsPipeline->mDescriptorSetLayouts.at(0).get()
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
                    mDescriptorSets.at(0).get(),
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
                    mDescriptorSets.at(0).get(),
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