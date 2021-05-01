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
        // Bind 0: Output image
        vk::initializers::DescriptorSetLayoutBinding(vk::DescriptorType::eStorageImage, vk::ShaderStageFlagBits::eCompute, 0),
        // Bind 1: Camera
        vk::initializers::DescriptorSetLayoutBinding(vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eCompute, 1),
        // Bind 2: Lights
        vk::initializers::DescriptorSetLayoutBinding(vk::DescriptorType::eUniformBuffer, vk::ShaderStageFlagBits::eCompute, 2),
        // Bind 3: Spheres
        vk::initializers::DescriptorSetLayoutBinding(vk::DescriptorType::eStorageBuffer, vk::ShaderStageFlagBits::eCompute, 3),
        // Bind 4: Planes
        vk::initializers::DescriptorSetLayoutBinding(vk::DescriptorType::eStorageBuffer, vk::ShaderStageFlagBits::eCompute, 4),
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
    mDescriptorImageInfo.imageLayout = vk::ImageLayout::eGeneral;
    mDescriptorImageInfo.imageView = mTexture->GetImageView();
    mDescriptorImageInfo.sampler = mTexture->GetSampler();

    vk::WriteDescriptorSet descriptorWrite {};
    descriptorWrite.dstSet = mDescriptorSets.at(0).get();
    descriptorWrite.dstBinding = 0;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = vk::DescriptorType::eStorageImage;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pImageInfo = &mDescriptorImageInfo;

    mDevice.updateDescriptorSets(1, &descriptorWrite, 0, nullptr);

    vk::CommandBufferAllocateInfo cmdBufAllocateInfo {};
    cmdBufAllocateInfo.commandPool = VulkanContext::Get()->GetCommandPool(QueueFamilyType::COMPUTE);
    cmdBufAllocateInfo.level = vk::CommandBufferLevel::ePrimary;
    cmdBufAllocateInfo.commandBufferCount = 1;
    mCmdBuffers = mDevice.allocateCommandBuffersUnique(cmdBufAllocateInfo);
}

void VulkanComputePipeline::RecreatePipeline()
{
    return;
}
