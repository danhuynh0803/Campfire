#include "Renderer/RendererAPI.h"
#include "Renderer/Shader.h"

#include "OpenGL/OpenGLShader.h"

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


//================================================
//----------------Shader Manager------------------
//================================================
std::unordered_map<std::string, SharedPtr<Shader>> ShaderManager::shaderMap;

void ShaderManager::Add(const std::string& tag, SharedPtr<Shader> shader)
{
    shaderMap.insert(std::pair<std::string, SharedPtr<Shader>>(tag, shader));
}

SharedPtr<Shader> ShaderManager::Create(const std::string& tag, const std::string& vertPath, const std::string& fragPath)
{
    if (Exists(tag))
    {
        return Get(tag);
    }
    else
    {
        auto shader = Shader::Create(tag, vertPath, fragPath);
        Add(tag, shader);
        return shader;
    }
}

bool ShaderManager::Exists(const std::string& tag)
{
    return ( shaderMap.find(tag) != shaderMap.end() );
}

SharedPtr<Shader> ShaderManager::Get(const std::string& tag)
{
    if (Exists(tag))
    {
        return shaderMap.find(tag)->second;
    }
}

void ShaderManager::ReloadShaders()
{
    for (auto pair : shaderMap)
    {
        SharedPtr<Shader> s = pair.second;
        SharedPtr<Shader> reloadedShader = Shader::Create(s->GetName(), s->GetVertexName(), s->GetFragmentName());

        pair.second->SetRenderID(reloadedShader->GetRenderID());
    }
    LOG_INFO("Reloaded Shaders");
}
