#pragma once

#include <Vulkan/Vulkan.hpp>
#include "Vulkan/VulkanBuffer.h"
//#include "Vulkan/VulkanDevice.h"

class VulkanImGui
{
public:
    VulkanImGui();
    //~VulkanImgui() = default;

    void InitResources();
    void SetupRenderPass();
    void UpdateBuffers();
    void DrawFrame(vk::CommandBuffer cmdBuffer);

    struct PushConstBlock {
        glm::vec2 scale;
        glm::vec2 translate;
    } mPushConstBlock;

private:

public:
    uint32_t mVertexCount;
    uint32_t mIndexCount;

    SharedPtr<VulkanBuffer> mVertexBuffer;
    SharedPtr<VulkanBuffer> mIndexBuffer;

    vk::UniqueImage mFontImage;
    vk::UniqueDeviceMemory mFontMemory;
    vk::UniqueImageView mFontImageView;
    vk::UniqueSampler mSampler;

    vk::UniquePipelineCache mPipelineCache;
    vk::UniquePipeline mPipeline;
    vk::UniquePipelineLayout mPipelineLayout;
    vk::UniqueRenderPass mRenderPass;

    vk::UniqueDescriptorPool mDescriptorPool;
    vk::UniqueDescriptorSetLayout mDescriptorSetLayout;
    std::vector<vk::UniqueDescriptorSet> mDescriptorSets;

    vk::Device* mDevicePtr = nullptr;
};
