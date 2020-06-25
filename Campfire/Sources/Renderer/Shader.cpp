#include "Renderer/Renderer.h"
#include "Renderer/Shader.h"

#include "Platform/OpenGL/OpenGLShader.h"

Shader* Shader::Create(const std::string& filepath)
{
    switch (Renderer::GetAPI())
    {
        case RendererAPI::None:
            return nullptr;
        case RendererAPI::OpenGL:
            return new OpenGLShader(filepath);
    }

    return nullptr;
}

Shader* Shader::Create(const std::string& name, const std::string& vertPath, const std::string& fragPath)
{
    switch (Renderer::GetAPI())
    {
        case RendererAPI::None:
            return nullptr;
        case RendererAPI::OpenGL:
            return new OpenGLShader(name, vertPath, fragPath);
    }

    return nullptr;
}

