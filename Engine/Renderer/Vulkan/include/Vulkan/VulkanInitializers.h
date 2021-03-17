#pragma once

#include <vulkan/vulkan.hpp>
#include <fstream>

namespace vk
{
    namespace initializers
    {
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
