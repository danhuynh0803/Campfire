#include "Vulkan/VulkanShader.h"
#include "Vulkan/VulkanContext.h"
#include <fstream>

VulkanShader::VulkanShader(const std::string& filepath)
    : name(filepath)
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

    shaderModule = VulkanContext::Get()->GetDevice()->GetVulkanDevice().createShaderModuleUnique(shaderInfo);
}
