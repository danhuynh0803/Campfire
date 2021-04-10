#include "Vulkan/VulkanComputePipeline.h"
#include "Vulkan/VulkanContext.h"
#include "Vulkan/VulkanUtil.h"
#include "Vulkan/VulkanInitializers.h"
#include "Core/ResourceManager.h"

VulkanComputePipeline::VulkanComputePipeline()
{
    auto device = VulkanContext::Get()->GetDevice()->GetVulkanDevice();
    auto swapChainImages = VulkanContext::Get()->GetSwapChain()->GetImages();

    // TODO just have a single pool
    { // Creating descriptor pools
        std::array<vk::DescriptorPoolSize, 1> poolSizes {};
        poolSizes[0].type = vk::DescriptorType::eCombinedImageSampler;
        poolSizes[0].descriptorCount = static_cast<uint32_t>(swapChainImages.size());

        vk::DescriptorPoolCreateInfo poolInfo;
        poolInfo.maxSets = 10000 * static_cast<uint32_t>(swapChainImages.size());
        poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();

        mDescriptorPool = device.createDescriptorPoolUnique(poolInfo);
    }

    std::vector<vk::DescriptorSetLayoutBinding> setLayoutBindings = {
        // Bind 0: Input image
        vk::initializers::DescriptorSetLayoutBinding(vk::DescriptorType::eStorageImage, vk::ShaderStageFlagBits::eCompute, 0),
        // Bind 1: Output image
        vk::initializers::DescriptorSetLayoutBinding(vk::DescriptorType::eStorageImage, vk::ShaderStageFlagBits::eCompute, 1),
    };

    auto setLayoutInfo = vk::initializers::DescriptorSetLayoutCreateInfo(
        static_cast<uint32_t>(setLayoutBindings.size()),
        setLayoutBindings.data());

    mDescriptorSetLayout = device.createDescriptorSetLayoutUnique(setLayoutInfo);

    auto pipelineLayoutInfo = vk::initializers::PipelineLayoutCreateInfo(1, &mDescriptorSetLayout.get());
    mPipelineLayout = device.createPipelineLayoutUnique(pipelineLayoutInfo);

    const auto swapChainSize = VulkanContext::Get()->GetSwapChain()->GetImages().size();
    std::vector<vk::DescriptorSetLayout> layouts { swapChainSize, mDescriptorSetLayout.get() };
    auto allocInfo = vk::initializers::DescriptorSetAllocateInfo(
        mDescriptorPool.get(),
        static_cast<uint32_t>(layouts.size()),
        layouts.data());

    mDescriptorSets = device.allocateDescriptorSetsUnique(allocInfo);

    auto computeShaderInfo = vk::initializers::PipelineShaderStageCreateInfo(
        SHADERS + "/invert.comp.spv",
        vk::ShaderStageFlagBits::eCompute);

    vk::ComputePipelineCreateInfo pipelineCreateInfo {};
    pipelineCreateInfo.stage = computeShaderInfo;
    pipelineCreateInfo.layout = mPipelineLayout.get();
    vk::PipelineCache pipelineCache;
    mPipeline = device.createComputePipelineUnique(pipelineCache, pipelineCreateInfo);
}

void VulkanComputePipeline::RecreatePipeline()
{
    return;
}
