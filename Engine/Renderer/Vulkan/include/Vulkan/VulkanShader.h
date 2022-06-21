#pragma once

#include <vulkan/vulkan.hpp>
#include "Renderer/Shader.h"

class VulkanShader
{
public:
    VulkanShader(const std::string& filepath);
    vk::ShaderModule GetShaderModule() const { return mShaderModule.get(); }
    uint32_t GetSize() const { return mSize; }

private:
    vk::UniqueShaderModule mShaderModule;
    std::string mName;
    uint32_t mSize;
};
