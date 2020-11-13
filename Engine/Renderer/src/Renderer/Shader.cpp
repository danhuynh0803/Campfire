#include "Renderer/RendererAPI.h"
#include "Renderer/Shader.h"
#include "OpenGL/OpenGLShader.h"
#include "Core/ResourceManager.h"

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

void ShaderManager::Init()
{
    ShaderManager::Create("line"  , SHADERS + "line.vert", SHADERS + "line.frag");
    ShaderManager::Create("pbr"   , SHADERS + "pbr.vert" , SHADERS + "pbr.frag");
    ShaderManager::Create("quad"  , SHADERS + "sprite_default.vert", SHADERS + "sprite_default.frag");
    ShaderManager::Create("text"  , SHADERS + "text.vert", SHADERS + "text.frag");
    ShaderManager::Create("skybox", SHADERS + "skybox.vert", SHADERS + "skybox.frag");
}

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
    // To be used primarily by engine shaders since these will be loaded
    // within the lib at the start

    // Probably a better way of doing this
    // but this helps to minimize the duplication of hardcoded shader file names
    if (Exists(tag))
    {
        return shaderMap.find(tag)->second;
    }
    else
    {
        LOG_ERROR("No shader named {0} is found");
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
