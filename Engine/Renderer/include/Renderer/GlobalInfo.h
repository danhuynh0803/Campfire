#pragma once

#include "Vulkan/VulkanContext.h"
#include "Scene/Camera.h"
#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include "Scene/Component.h"

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
} // namespace global

namespace rt
{

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
    glm::vec4 emission;
    glm::vec4 albedo;
    int mat;
    int id;
    glm::vec2 padding;
};

//========================================================
struct Plane
{
    glm::vec3 normal;
    float distance;
    glm::vec3 albedo;
    float specular;
    int mat;
    int id;
    glm::vec2 padding;
};

struct RayTraceScene
{
    std::vector<Sphere> spheres;
    SharedPtr<VulkanBuffer> sphereSSBO;
    std::vector<Plane> planes;
    SharedPtr<VulkanBuffer> planeSSBO;

    void Init()
    {
        // SCENE
        // { float radius,          { float3 position },        { float4 emission },          { float4 albedo },       material }
        //int id = 0;
        //spheres = {
        //    { 1  , {  0, 0, 0    }, { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.85f, .35f, .35f, 1.0f }, MatType::DIFF, id++ }, // Left
        //    { 1e5f  , {  1e5f + 1.0f, 40.8f, 81.6f    }, { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.85f, .35f, .35f, 1.0f }, MatType::DIFF, id++ }, // Left
        //    { 1e5f  , { -1e5f + 99.0f, 40.8f, 81.6f   }, { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.35f, .35f, .85f, 1.0f }, MatType::DIFF, id++ }, // Right
        //    { 1e5f  , {  50.0f, 40.8f, 1e5f           }, { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.75f, .75f, .75f, 1.0f }, MatType::DIFF, id++ }, // Back
        //    { 1e5f  , {  50.0f, 40.8f, -1e5f + 600.0f }, { 0.0f, 0.0f, 0.0f, 0.0f }, { 1.00f, 1.0f, 1.0f, 1.0f }, MatType::DIFF, id++ }, // Frnt
        //    { 1e5f  , {  50.0f, 1e5f, 81.6f           }, { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.75f, .75f, .75f, 1.0f }, MatType::DIFF, id++ }, // Botm
        //    { 1e5f  , {  50.0f, -1e5f + 81.6f, 81.6f  }, { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.75f, .75f, .75f, 1.0f }, MatType::DIFF, id++ }, // Top
        //    { 16.5f , {  27.0f, 16.5f, 47.0f          }, { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.90f, 0.1f, 0.1f, 1.0f }, MatType::DIFF, id++ }, // small sphere 1
        //    { 16.5f , {  73.0f, 16.5f, 78.0f          }, { 0.0f, 0.0f, 0.0f, 0.0f }, { 0.10f, 0.3f, 1.0f, 1.0f }, MatType::SPEC, id++ }, // small sphere 2
        //    { 600.0f, {  50.0f, 681.6f - .77f, 81.6f  }, { 2.0f, 1.8f, 1.6f, 0.0f }, { 0.00f, 0.0f, 0.0f, 1.0f }, MatType::DIFF, id++ }, // Light
        //};


        int maxRow = 10;
        int maxCol = 10;
        int id = 0;
        for (int i = 0; i < maxRow; ++i)
        {
            for (int j = 0; j < maxCol; ++j)
            {
                Sphere sphere{};
                sphere.radius = 0.5f;
                sphere.pos = glm::vec3(j, i, 0);
                sphere.emission = glm::vec4(0.0f);
                sphere.albedo = glm::vec4((float)j / maxCol, (float)i / maxRow, 1.0f, 1.0f);
                sphere.mat = 0;
                sphere.id = id++;

                spheres.push_back(sphere);
            }
        }


        planes = {
            { glm::vec3(0, 0, -7), 5, glm::vec3(0, 0, 1), 32, MatType::DIFF, 0 },
        };
    }

    //auto graphicsPipeline = VulkanContext::Get()->mFrameGraph.GetGraphicsPipeline("PostProcess");
    //auto computePipeline = VulkanContext::Get()->mComputePipeline;

    void Update()
    {
        //{ // -- sphere SSBO
        //    // TODO: No need to flush since coherent, but investigate
        //    // what the performance hit is since we're not using
        //    // device local memory
        //    sphereSSBO = CreateSharedPtr<VulkanBuffer>(
        //        vk::BufferUsageFlagBits::eStorageBuffer,
        //        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        //        spheres.size() * sizeof(Sphere),
        //        vk::SharingMode::eExclusive
        //        );

        //    void* data = sphereSSBO->Map();
        //    memcpy(data, spheres.data(), spheres.size() * sizeof(Sphere));

        //    // Update DescriptorSet
        //    vk::DescriptorBufferInfo bufferInfo{};
        //    bufferInfo.buffer = sphereSSBO->mBuffer.get();
        //    bufferInfo.offset = 0;
        //    bufferInfo.range = sphereSSBO->mSize;

        //    vk::WriteDescriptorSet writeInfo{};
        //    writeInfo.dstSet = computePipeline->mDescriptorSets.at(0).get();
        //    writeInfo.dstBinding = 3;
        //    writeInfo.dstArrayElement = 0;
        //    writeInfo.descriptorType = vk::DescriptorType::eStorageBuffer;
        //    writeInfo.descriptorCount = 1;
        //    writeInfo.pBufferInfo = &bufferInfo;

        //    mDevice.updateDescriptorSets(1, &writeInfo, 0, nullptr);
        //}

        //{ // -- plane SSBO
        //    planeSSBO = CreateSharedPtr<VulkanBuffer>(
        //        vk::BufferUsageFlagBits::eStorageBuffer,
        //        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        //        planes.size() * sizeof(Plane),
        //        vk::SharingMode::eExclusive
        //        );

        //    void* data = planeSSBO->Map();
        //    memcpy(data, planes.data(), planes.size() * sizeof(Plane));

        //    // Update DescriptorSet
        //    vk::DescriptorBufferInfo bufferInfo{};
        //    bufferInfo.buffer = sphereSSBO->mBuffer.get();
        //    bufferInfo.offset = 0;
        //    bufferInfo.range = sphereSSBO->mSize;

        //    vk::WriteDescriptorSet writeInfo{};
        //    writeInfo.dstSet = computePipeline->mDescriptorSets.at(0).get();
        //    writeInfo.dstBinding = 4;
        //    writeInfo.dstArrayElement = 0;
        //    writeInfo.descriptorType = vk::DescriptorType::eStorageBuffer;
        //    writeInfo.descriptorCount = 1;
        //    writeInfo.pBufferInfo = &bufferInfo;

        //    mDevice.updateDescriptorSets(1, &writeInfo, 0, nullptr);
        //}

        //// Update post compute graphics descriptorset that reads in the processed image
        //vk::WriteDescriptorSet writeInfo{};
        //writeInfo.dstSet = graphicsPipeline->mDescriptorSets[1][0].get();
        //writeInfo.dstBinding = 0;
        //writeInfo.dstArrayElement = 0;
        //writeInfo.descriptorCount = 1;
        //writeInfo.descriptorType = vk::DescriptorType::eCombinedImageSampler;
        //writeInfo.pImageInfo = &computePipeline->mDescriptorImageInfo;

        //mDevice.updateDescriptorSets(1, &writeInfo, 0, nullptr);
    }

    //vk::FenceCreateInfo fenceInfo;
    //fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;
    //computeFence = mDevice.createFenceUnique(fenceInfo);

    //auto computePipeline = VulkanContext::Get()->mComputePipeline;
    //vk::CommandBufferBeginInfo cmdBufferInfo{};
    //auto& cmdBuffer = computePipeline->mCmdBuffers.at(0);
    //// Dispatch compute command
    //cmdBuffer->begin(cmdBufferInfo);
    //cmdBuffer->bindPipeline(vk::PipelineBindPoint::eCompute, computePipeline->mPipeline.get());
    //cmdBuffer->bindDescriptorSets(
    //    vk::PipelineBindPoint::eCompute,
    //    computePipeline->mPipelineLayout.get(),
    //    0,
    //    1, &computePipeline->mDescriptorSets.at(0).get(),
    //    0, nullptr
    //);
    //cmdBuffer->dispatch(computePipeline->mTexture->GetWidth() / 16, computePipeline->mTexture->GetHeight() / 16, 1);
    //cmdBuffer->end();

    //// Submit compute command
    //vk::SubmitInfo computeSubmitInfo{};
    //computeSubmitInfo.commandBufferCount = 1;
    //computeSubmitInfo.pCommandBuffers = &VulkanContext::Get()->mComputePipeline->mCmdBuffers.at(0).get();

    //mDevice.waitForFences(computeFence.get(), VK_TRUE, UINT64_MAX);
    //mDevice.resetFences(computeFence.get());
    //auto computeQueue = VulkanContext::Get()->GetDevice()->GetQueue(QueueFamilyType::COMPUTE);
    //computeQueue.submit(computeSubmitInfo, computeFence.get());

    //// TODO add image barrier from compute to fragment

    //for (size_t frame = 0; frame < 3; ++frame)
    //{
    //    auto commandBuffer = VulkanRenderer::BeginScene(frame);
    //    {
    //        mPushConstBlock.model = glm::mat4(1.0f);
    //        commandBuffer.pushConstants(
    //            VulkanContext::Get()->mFrameGraph.GetGraphicsPipeline("PostProcess")->mPipelineLayout.get(),
    //            vk::ShaderStageFlagBits::eVertex,
    //            0, sizeof(VulkanGraphicsPipeline::TransformPushConstBlock),
    //            &mPushConstBlock
    //        );

    //        commandBuffer.bindDescriptorSets(
    //            vk::PipelineBindPoint::eGraphics,
    //            VulkanContext::Get()->mFrameGraph.GetGraphicsPipeline("PostProcess")->mPipelineLayout.get(),
    //            1,
    //            1,
    //            &VulkanContext::Get()->mFrameGraph.GetGraphicsPipeline("PostProcess")->mDescriptorSets[1][0].get(),
    //            0,
    //            nullptr
    //        );

    //        VulkanRenderer::DrawIndexed(commandBuffer, vertexBufferPtr->GetBuffer(), indexBufferPtr->GetBuffer(), indexBufferPtr->GetCount());

    //        vkImguiLayer->mImGuiImpl->DrawFrame(commandBuffer);
    //    }
    //    VulkanRenderer::EndScene(commandBuffer);
    //}

    //}
};

} // namespace raytrace
