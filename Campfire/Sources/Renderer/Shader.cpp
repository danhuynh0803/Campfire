#include "Renderer/RendererAPI.h"
#include "Renderer/Shader.h"

#include "Platform/OpenGL/OpenGLShader.h"

SharedPtr<Shader> Shader::Create(const std::string& filepath)
{
    switch (RendererAPI::GetAPI())
    {
        case RendererAPI::API::None:
            return nullptr;
        case RendererAPI::API::OpenGL:
            return CreateSharedPtr<OpenGLShader>(filepath);
    }

    return nullptr;
}

SharedPtr<Shader> Shader::Create(const std::string& name, const std::string& vertPath, const std::string& fragPath)
{
    switch (RendererAPI::GetAPI())
    {
        case RendererAPI::API::None:
            return nullptr;
        case RendererAPI::API::OpenGL:
            return CreateSharedPtr<OpenGLShader>(name, vertPath, fragPath);
    }

    return nullptr;
}

