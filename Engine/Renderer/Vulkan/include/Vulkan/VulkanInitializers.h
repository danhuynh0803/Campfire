#pragma once

#include <vulkan/vulkan.hpp>
#include <fstream>

namespace vk::initializers
{
    inline vk::BufferCreateInfo BufferCreateInfo(
        vk::BufferUsageFlags usage,
        vk::SharingMode sharingMode,
        vk::DeviceSize size,
        vk::BufferCreateFlags flags = vk::BufferCreateFlags())
    {
        vk::BufferCreateInfo bufferCreateInfo {};
        bufferCreateInfo.size = size;
        bufferCreateInfo.usage = usage;
        bufferCreateInfo.sharingMode = sharingMode;
        bufferCreateInfo.flags = flags;

        return bufferCreateInfo;
    }

    inline vk::VertexInputBindingDescription VertexInputBindingDescription(
            uint32_t binding,
            uint32_t stride,
            vk::VertexInputRate inputRate)
    {
        vk::VertexInputBindingDescription description{};
        description.binding = binding;
        description.stride = stride;
        description.inputRate = inputRate;

        return description;
    }

    inline vk::VertexInputAttributeDescription VertexInputAttributeDescription(
        uint32_t binding,
        uint32_t location,
        vk::Format format,
        uint32_t offset)
    {
        vk::VertexInputAttributeDescription description{};
        description.binding = binding;
        description.location = location;
        description.format = format;
        description.offset = offset;

        return description;
    }

    inline vk::PipelineVertexInputStateCreateInfo PipelineVertexInputStateCreateInfo(
        uint32_t vertexBindingDescriptionCount,
        vk::VertexInputBindingDescription* pVertexBindingDescriptions,
        uint32_t vertexAttributeDescriptionCount,
        vk::VertexInputAttributeDescription* pVertexAttributeDescriptions,
        vk::PipelineVertexInputStateCreateFlags flags = vk::PipelineVertexInputStateCreateFlags())
    {
        vk::PipelineVertexInputStateCreateInfo info;
        info.vertexBindingDescriptionCount = vertexBindingDescriptionCount;
        info.pVertexBindingDescriptions = pVertexBindingDescriptions;
        info.vertexAttributeDescriptionCount = vertexAttributeDescriptionCount;
        info.pVertexAttributeDescriptions = pVertexAttributeDescriptions;
        info.flags = flags;

        return info;
    }

    inline vk::PipelineInputAssemblyStateCreateInfo PipelineInputAssemblyStateCreateInfo(
        vk::PrimitiveTopology topology,
        vk::Bool32 primitiveRestartEnable,
        vk::PipelineInputAssemblyStateCreateFlags flags = vk::PipelineInputAssemblyStateCreateFlags())
    {
        vk::PipelineInputAssemblyStateCreateInfo info;
        info.topology = topology;
        info.primitiveRestartEnable = primitiveRestartEnable;
        info.flags = flags;

        return info;
    }

    inline vk::Viewport Viewport(
        float x, float y,
        float width, float height,
        float minDepth, float maxDepth)
    {
        vk::Viewport viewport;
        viewport.x = x;
        viewport.y = y;
        viewport.width = width;
        viewport.height = height;
        viewport.minDepth = minDepth;
        viewport.maxDepth = maxDepth;

        return viewport;
    }

    inline vk::PipelineViewportStateCreateInfo PipelineViewportStateCreateInfo(
        uint32_t viewportCount,
        vk::Viewport* pViewports,
        uint32_t scissorCount,
        vk::Rect2D* pScissors,
        vk::PipelineViewportStateCreateFlags flags = vk::PipelineViewportStateCreateFlags())
    {
        vk::PipelineViewportStateCreateInfo pipelineViewportStateCreateInfo {};
        pipelineViewportStateCreateInfo.viewportCount = viewportCount;
        pipelineViewportStateCreateInfo.pViewports = pViewports;
        pipelineViewportStateCreateInfo.scissorCount = scissorCount;
        pipelineViewportStateCreateInfo.pScissors = pScissors;
        pipelineViewportStateCreateInfo.flags = flags;

        return pipelineViewportStateCreateInfo;
    }

    inline vk::PipelineRasterizationStateCreateInfo PipelineRasterizationStateCreateInfo(
        vk::PolygonMode polygonMode,
        vk::CullModeFlagBits cullMode,
        vk::FrontFace frontFace,
        vk::PipelineRasterizationStateCreateFlags flags = vk::PipelineRasterizationStateCreateFlags())
    {
        vk::PipelineRasterizationStateCreateInfo info;
        info.polygonMode = polygonMode;
        info.cullMode = cullMode;
        info.frontFace = frontFace;
        info.depthClampEnable = VK_FALSE;
        info.rasterizerDiscardEnable = VK_FALSE;
        info.depthBiasEnable = VK_FALSE;
        info.depthBiasConstantFactor = 0.0f;
        info.depthBiasClamp = 0.0f;
        info.depthBiasSlopeFactor = 0.0f;
        info.lineWidth = 1.0f;
        info.flags = flags;

        return info;
    }

    inline vk::PipelineMultisampleStateCreateInfo PipelineMultisampleStateCreateInfo(
        vk::SampleCountFlagBits rasterizationSamples,
        vk::PipelineMultisampleStateCreateFlags flags = vk::PipelineMultisampleStateCreateFlags())
    {
        vk::PipelineMultisampleStateCreateInfo info {};
        info.rasterizationSamples = rasterizationSamples;
        info.flags = flags;

        info.sampleShadingEnable = VK_FALSE;
        info.minSampleShading = 1.0f;
        info.pSampleMask = nullptr;
        info.alphaToCoverageEnable = VK_FALSE;
        info.alphaToOneEnable = VK_FALSE;

        return info;
    }

    inline vk::PipelineColorBlendAttachmentState PipelineColorBlendAttachmentState(
        vk::ColorComponentFlags colorWriteMask,
        vk::Bool32 blendEnable)
    {
        vk::PipelineColorBlendAttachmentState state {};
        state.colorWriteMask = colorWriteMask;
        state.blendEnable = blendEnable;

        return state;
    }

    inline vk::PipelineColorBlendStateCreateInfo PipelineColorBlendStateCreateInfo(
        uint32_t attachmentCount,
        vk::PipelineColorBlendAttachmentState* pAttachments,
        vk::PipelineColorBlendStateCreateFlags flags = vk::PipelineColorBlendStateCreateFlags())
    {
        vk::PipelineColorBlendStateCreateInfo info {};
        info.attachmentCount = attachmentCount;
        info.pAttachments = pAttachments;
        info.flags = flags;
        info.logicOpEnable = VK_FALSE;
        //info.logicOp = vk::LogicOp::eCopy;

        return info;
    }

    inline vk::PipelineDynamicStateCreateInfo PipelineDynamicStateCreateInfo(
        uint32_t dynamicStateCount,
        vk::DynamicState* pDynamicStates,
        vk::PipelineDynamicStateCreateFlags flags = vk::PipelineDynamicStateCreateFlags())
    {
        vk::PipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo {};
        pipelineDynamicStateCreateInfo.dynamicStateCount = dynamicStateCount;
        pipelineDynamicStateCreateInfo.pDynamicStates = pDynamicStates;
        pipelineDynamicStateCreateInfo.flags = flags;

        return pipelineDynamicStateCreateInfo;
    }


    inline vk::PipelineDepthStencilStateCreateInfo PipelineDepthStencilStateCreateInfo(
        vk::Bool32 depthTestEnable,
        vk::Bool32 depthWriteEnable,
        vk::CompareOp depthCompareOp)
    {
        vk::PipelineDepthStencilStateCreateInfo info {};
        info.depthTestEnable = depthTestEnable;
        info.depthWriteEnable = depthWriteEnable;

        info.depthCompareOp = depthCompareOp;
        info.depthBoundsTestEnable = false;
        info.minDepthBounds = 0.0f;
        info.maxDepthBounds = 1.0f;
        info.stencilTestEnable = false;
        info.front = vk::StencilOpState{};
        info.back  = vk::StencilOpState{};

        return info;
    }

    inline vk::DescriptorSetLayoutBinding DescriptorSetLayoutBinding(
        vk::DescriptorType descriptorType,
        vk::ShaderStageFlags stageFlags,
        uint32_t binding,
        uint32_t descriptorCount = 1)
    {
        vk::DescriptorSetLayoutBinding descriptorSetLayout {};
        descriptorSetLayout.binding = binding;
        descriptorSetLayout.descriptorType = descriptorType;
        descriptorSetLayout.descriptorCount = descriptorCount;
        descriptorSetLayout.stageFlags = stageFlags;
        descriptorSetLayout.pImmutableSamplers = nullptr;

        return descriptorSetLayout;
    }

    inline vk::DescriptorSetAllocateInfo DescriptorSetAllocateInfo(
        vk::DescriptorPool pool,
        uint32_t descriptorSetCount,
        vk::DescriptorSetLayout* pSetLayouts)
    {
        vk::DescriptorSetAllocateInfo descriptorSetAllocateInfo {};
        descriptorSetAllocateInfo.descriptorPool = pool;
        descriptorSetAllocateInfo.descriptorSetCount = descriptorSetCount;
        descriptorSetAllocateInfo.pSetLayouts = pSetLayouts;

        return descriptorSetAllocateInfo;
    }

    inline vk::DescriptorSetLayoutCreateInfo DescriptorSetLayoutCreateInfo(
        uint32_t bindingCount,
        vk::DescriptorSetLayoutBinding* pBindings)
    {
        vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo {};
        descriptorSetLayoutCreateInfo.bindingCount = bindingCount;
        descriptorSetLayoutCreateInfo.pBindings = pBindings;

        return descriptorSetLayoutCreateInfo;
    }

    inline vk::PipelineLayoutCreateInfo PipelineLayoutCreateInfo(
        uint32_t setLayoutCount,
        vk::DescriptorSetLayout* pSetLayouts)
    {
        vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo {};
        pipelineLayoutCreateInfo.setLayoutCount = setLayoutCount;
        pipelineLayoutCreateInfo.pSetLayouts = pSetLayouts;

        return pipelineLayoutCreateInfo;
    }

    /*
    inline vk::ShaderModule LoadShaderModule(const std::string& filepath)
    {
        std::ifstream file(filepath, std::ios::ate | std::ios::binary);

        if (!file.is_open())
        {
            throw std::runtime_error("VulkanShader::Failed to open " + filepath);
        }

        size_t fileSize = (size_t)file.tellg();
        std::vector<char> buffer;
        buffer.resize(fileSize);

        file.seekg(0);
        file.read(buffer.data(), fileSize);

        file.close();

        vk::ShaderModuleCreateInfo shaderInfo;
        shaderInfo.flags = vk::ShaderModuleCreateFlags();
        shaderInfo.codeSize = buffer.size();
        shaderInfo.pCode = reinterpret_cast<const uint32_t*>(buffer.data());

        return VulkanContext::Get()->GetDevice()->GetVulkanDevice().createShaderModule(shaderInfo);
    }

    inline vk::PipelineShaderStageCreateInfo PipelineShaderStageCreateInfo(
        const std::string& filepath,
        vk::ShaderStageFlagBits stage)
    {
        vk::PipelineShaderStageCreateInfo shaderStageInfo;
        shaderStageInfo.stage = stage;
        shaderStageInfo.module = LoadShaderModule(filepath);
        shaderStageInfo.pName = "main";

        return shaderStageInfo;
    }
    */
}
