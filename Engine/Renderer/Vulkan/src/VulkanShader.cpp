#include "Vulkan/VulkanShader.h"
#include "Vulkan/VulkanContext.h"
#include "Core/ResourceManager.h"
#include <shaderc/shaderc.hpp>
#include <fstream>

std::vector<uint32_t> VulkanShader::CompileFile(
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
        // TODO if compilation fails, use a shader to indicate something is broken
        // like the purple shader in unity

        // If not compiled successfully, return the last working module
        // rather than just crashing from an invalid module
        return ResourceManager::mCompiledShaders[filepath];
    }

    // Update with latest successfully compiled module
    ResourceManager::mCompiledShaders[filepath] = { module.cbegin(), module.cend() };

    return { module.cbegin(), module.cend() };
}

shaderc_shader_kind GetShaderKindFromFileName(const std::string& filepath)
{
    const auto lastDot = filepath.find_last_of('.');
    const auto ext = filepath.substr(lastDot + 1);

    shaderc_shader_kind kind = shaderc_shader_kind::shaderc_glsl_infer_from_source;

    if (ext == "comp")
        kind = shaderc_shader_kind::shaderc_glsl_default_compute_shader;
    else if (ext == "vert")
        kind = shaderc_shader_kind::shaderc_glsl_default_vertex_shader;
    else if (ext == "frag")
        kind = shaderc_shader_kind::shaderc_glsl_default_fragment_shader;
    else if (ext == "geom")
        kind = shaderc_shader_kind::shaderc_glsl_default_geometry_shader;
    else if (ext == "mesh")
        kind = shaderc_shader_kind::shaderc_glsl_default_mesh_shader;
    // TODO raytracing shader kind

    return kind;
}

std::string GetShaderSource(const std::string& filepath)
{
    std::string shaderSource;
    std::ifstream fileStream;
    fileStream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        fileStream.open(filepath);
        std::stringstream shaderStream;
        shaderStream << fileStream.rdbuf();
        fileStream.close();
        shaderSource = shaderStream.str();
    } catch (std::ifstream::failure e) {
        std::cerr << "ERROR Shader file not successfully read: " << filepath;
    }

    return shaderSource;
}

VulkanShader::VulkanShader(const std::string& filepath)
    : mName(filepath)
{
    const auto lastDot = filepath.find_last_of('.');
    const auto ext = filepath.substr(lastDot + 1);

    // Check type of shader
    // if file is already spirv, then proceed as normal
    if (ext == "spv")
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
        shaderInfo.flags    = vk::ShaderModuleCreateFlags();
        shaderInfo.codeSize = buffer.size();
        shaderInfo.pCode    = reinterpret_cast<const uint32_t*>(buffer.data());
        mShaderModule = VulkanContext::Get()->GetDevice()->GetVulkanDevice().createShaderModuleUnique(shaderInfo);
    }
    else
    {
        // compile the glsl file
        auto shaderKind   = GetShaderKindFromFileName(filepath);
        auto shaderSrc    = GetShaderSource(filepath);
        auto binary       = CompileFile(filepath, shaderKind, shaderSrc, false);

        vk::ShaderModuleCreateInfo shaderInfo {};
        shaderInfo.flags    = vk::ShaderModuleCreateFlags();
        shaderInfo.codeSize = binary.size() * sizeof(uint32_t);
        shaderInfo.pCode    = binary.data();
        mShaderModule = VulkanContext::Get()->GetDevice()->GetVulkanDevice().createShaderModuleUnique(shaderInfo);
    }
}
