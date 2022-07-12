#pragma once

#include <vulkan/vulkan.hpp>
#include <shaderc/shaderc.hpp>
#include "Renderer/Shader.h"

class VulkanShader
{
public:
    VulkanShader(const std::string& filepath);
    vk::ShaderModule GetShaderModule() const { return mShaderModule.get(); }
    uint32_t GetSize() const { return mSize; }

private:
    std::vector<uint32_t> CompileFile(
        const std::string& filepath,
        shaderc_shader_kind kind,
        const std::string& source,
        bool optimize);

private:
    vk::UniqueShaderModule mShaderModule;
    std::string mName;
    uint32_t mSize;
};

class Program
{

};
