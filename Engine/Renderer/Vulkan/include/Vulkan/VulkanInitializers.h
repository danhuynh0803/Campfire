#pragma once

#include <vulkan/vulkan.hpp>
#include <fstream>

namespace vk
{
    namespace initializers
    {
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
            uint32_t descriptionCount,
            vk::PipelineVertexInputStateCreateFlags flags = vk::PipelineVertexInputStateCreateFlags())
        {
            vk::PipelineVertexInputStateCreateInfo info;
            info.flags = flags;
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
            uint32_t scissorCount,
            vk::PipelineViewportStateCreateFlags flags = vk::PipelineViewportStateCreateFlags())
        {
            vk::PipelineViewportStateCreateInfo info;
            info.viewportCount = viewportCount;
            info.scissorCount = scissorCount;
            info.flags = flags;

            return info;
        }

        inline vk::PipelineMultisampleStateCreateInfo PipelineMultisampleStateCreateInfo(
            vk::SampleCountFlagBits rasterizationSamples,
            vk::PipelineMultisampleStateCreateFlags flags = vk::PipelineMultisampleStateCreateFlags())
        {
            vk::PipelineMultisampleStateCreateInfo info;
            info.rasterizationSamples = rasterizationSamples;
            info.flags = flags;

            // TODO expose later, but not gonna use for a while
            //info.sampleShadingEnable = VK_FALSE;
            //info.minSampleShading = 1.0f;
            //info.pSampleMask = nullptr;
            //info.alphaToCoverageEnable = VK_FALSE;
            //info.alphaToOneEnable = VK_FALSE;

            return info;
        }

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



    }


}
