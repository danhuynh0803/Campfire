#include "Vulkan/VulkanShader.h"
#include "Vulkan/VulkanContext.h"
#include <shaderc/shaderc.hpp>
#include <fstream>

std::vector<uint32_t> CompileFile(
    const std::string& filepath,
    shaderc_shader_kind kind,
    const std::string& source,
    bool optimize = false
)
{
    shaderc::Compiler compiler;
    shaderc::CompileOptions options;

    if (optimize) {
        options.SetOptimizationLevel(shaderc_optimization_level_size);
    }

    shaderc::SpvCompilationResult module = 
        compiler.CompileGlslToSpv(source, kind, filepath.c_str(), options);

    if (module.GetCompilationStatus() != shaderc_compilation_status_success)
    {
        std::cerr << module.GetErrorMessage();
        return std::vector<uint32_t>();
    }

    return { module.cbegin(), module.cend() };
}

VulkanShader::VulkanShader(const std::string& filepath)
    : name(filepath)
{
    // Check type of shader



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
