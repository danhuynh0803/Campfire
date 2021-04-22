#include "Vulkan/VulkanComputePipeline.h"
#include "Vulkan/VulkanContext.h"
#include "Vulkan/VulkanUtil.h"
#include "Vulkan/VulkanInitializers.h"
#include "Core/ResourceManager.h"

VulkanComputePipeline::VulkanComputePipeline()
{
    mDevice = VulkanContext::Get()->GetDevice()->GetVulkanDevice();
    auto swapChainImages = VulkanContext::Get()->GetSwapChain()->GetImages();

    std::vector<vk::DescriptorSetLayoutBinding> setLayoutBindings = {
        // Bind 0: Input image
        //vk::initializers::DescriptorSetLayoutBinding(vk::DescriptorType::eStorageImage, vk::ShaderStageFlagBits::eCompute, 0),
        // Bind 1: Output image
        vk::initializers::DescriptorSetLayoutBinding(vk::DescriptorType::eStorageImage, vk::ShaderStageFlagBits::eCompute, 0),
    };

    auto setLayoutInfo = vk::initializers::DescriptorSetLayoutCreateInfo(
        static_cast<uint32_t>(setLayoutBindings.size()),
        setLayoutBindings.data());

    mDescriptorSetLayout = mDevice.createDescriptorSetLayoutUnique(setLayoutInfo);

    auto pipelineLayoutInfo = vk::initializers::PipelineLayoutCreateInfo(1, &mDescriptorSetLayout.get());
    mPipelineLayout = mDevice.createPipelineLayoutUnique(pipelineLayoutInfo);

    const auto swapChainSize = VulkanContext::Get()->GetSwapChain()->GetImages().size();
    std::vector<vk::DescriptorSetLayout> layouts { swapChainSize, mDescriptorSetLayout.get() };
    auto allocInfo = vk::initializers::DescriptorSetAllocateInfo(
        VulkanContext::Get()->GetDescriptorPool(),
        static_cast<uint32_t>(layouts.size()),
        layouts.data());

    mDescriptorSets = mDevice.allocateDescriptorSetsUnique(allocInfo);

    auto computeShaderInfo = vk::initializers::PipelineShaderStageCreateInfo(
        SHADERS + "/invert.comp.spv",
        vk::ShaderStageFlagBits::eCompute);

    vk::ComputePipelineCreateInfo pipelineCreateInfo {};
    pipelineCreateInfo.stage = computeShaderInfo;
    pipelineCreateInfo.layout = mPipelineLayout.get();
    vk::PipelineCache pipelineCache;
    mPipeline = mDevice.createComputePipelineUnique(pipelineCache, pipelineCreateInfo);

    // Create empty texture to bind for storing compute output
    mTexture = CreateSharedPtr<VulkanTexture2D>(1920, 1080);

    // Update descriptorSet with image
    vk::DescriptorImageInfo imageInfo {};
    imageInfo.imageLayout = vk::ImageLayout::eGeneral;
    imageInfo.imageView = mTexture->GetImageView();
    imageInfo.sampler = mTexture->GetSampler();

    vk::WriteDescriptorSet descriptorWrite {};
    descriptorWrite.dstSet = mDescriptorSets.at(0).get();
    descriptorWrite.dstBinding = 0;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = vk::DescriptorType::eStorageImage;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pImageInfo = &imageInfo;

    mDevice.updateDescriptorSets(1, &descriptorWrite, 0, nullptr);

    vk::CommandBufferAllocateInfo cmdBufAllocateInfo {};
    cmdBufAllocateInfo.commandPool = VulkanContext::Get()->GetCommandPool(QueueFamilyType::COMPUTE);
    cmdBufAllocateInfo.level = vk::CommandBufferLevel::ePrimary;
    cmdBufAllocateInfo.commandBufferCount = 1;
    mCmdBuffers = mDevice.allocateCommandBuffersUnique(cmdBufAllocateInfo);

    // Create command buffer and dispatch command
    vk::CommandBufferBeginInfo cmdBufferInfo {};
    auto& cmdBuffer = mCmdBuffers.at(0);
    cmdBuffer->begin(cmdBufferInfo);
    cmdBuffer->bindPipeline(vk::PipelineBindPoint::eCompute, mPipeline.get());
    cmdBuffer->bindDescriptorSets(vk::PipelineBindPoint::eCompute, mPipelineLayout.get(), 0, 1, &mDescriptorSets.at(0).get(), 0, 0);

    cmdBuffer->dispatch(mTexture->GetWidth() / 16, mTexture->GetHeight() / 16, 1);

    cmdBuffer->end();
}

void VulkanComputePipeline::RecreatePipeline()
{
    return;
}
