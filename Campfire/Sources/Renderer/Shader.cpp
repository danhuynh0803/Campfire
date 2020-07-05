#include "Renderer/RendererAPI.h"
#include "Renderer/Shader.h"

#include "Platform/OpenGL/OpenGLShader.h"

Shader* Shader::Create(const std::string& filepath)
{
    switch (RendererAPI::GetAPI())
    {
        case RendererAPI::API::None:
            return nullptr;
        case RendererAPI::API::OpenGL:
            return new OpenGLShader(filepath);
    }

    return nullptr;
}

Shader* Shader::Create(const std::string& name, const std::string& vertPath, const std::string& fragPath)
{
    switch (RendererAPI::GetAPI())
    {
        case RendererAPI::API::None:
            return nullptr;
        case RendererAPI::API::OpenGL:
            return new OpenGLShader(name, vertPath, fragPath);
    }

    return nullptr;
}

