#pragma once

#include <vulkan/vulkan.hpp>
#include "Renderer/Shader.h"

class VulkanShader
{
public:
    VulkanShader(const std::string& filepath);
    vk::ShaderModule GetShaderModule() { return shaderModule.get(); }

private:
    vk::UniqueShaderModule shaderModule;
    std::string name;
};
