#pragma once

#include <Vulkan/Vulkan.hpp>
#include "Vulkan/VulkanBuffer.h"
//#include "Vulkan/VulkanDevice.h"

class VulkanImGuiImpl
{
public:
    VulkanImGuiImpl();
    //~VulkanImgui() = default;

private:
    void InitResources();

private:
    SharedPtr<VulkanVertexBuffer> mVertexBuffer;
    SharedPtr<VulkanIndexBuffer> mIndexBuffer;

    vk::UniqueImage mFontImage;
    vk::UniqueDeviceMemory mFontMemory;
    vk::UniqueImageView mFontImageView;
    vk::UniqueSampler mSampler;

    vk::UniquePipelineCache mPipelineCache;
    vk::UniquePipeline mPipeline;
    vk::UniquePipelineLayout mPipelineLayout;

    vk::UniqueDescriptorPool mDescriptorPool;
    vk::UniqueDescriptorSetLayout mDescriptorSetLayout;
    std::vector<vk::UniqueDescriptorSet> mDescriptorSets;

    vk::Device* mDevicePtr;

} typedef VulkanImGui;
