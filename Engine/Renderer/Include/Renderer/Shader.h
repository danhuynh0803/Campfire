#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <glm/glm.hpp>
#include <unordered_map>

#include "Core/Base.h"

class Shader
{
public:
    static SharedPtr<Shader> Create(const std::string& filepath);
    static SharedPtr<Shader> Create(const std::string& name, const std::string& vertPath, const std::string& fragPath);

    virtual void SetBool(const std::string& name, bool value) = 0;

    virtual void SetInt(const std::string& name, int value) = 0;
    virtual void SetIntArray(const std::string& name, int* values, uint32_t count) = 0;

    virtual void SetFloat(const std::string& name, float value) = 0;
    virtual void SetFloat3(const std::string& name, const glm::vec3& value) = 0;
    virtual void SetFloat4(const std::string& name, const glm::vec4& value) = 0;

    virtual void SetMat4(const std::string& name, const glm::mat4& value) = 0;

    virtual void Bind() = 0;
    virtual void Unbind() = 0;

    virtual const std::string& GetName() const = 0;
    virtual const std::string& GetVertexName() const = 0;
    virtual const std::string& GetFragmentName() const = 0;

    virtual uint32_t GetRenderID() const = 0;
    virtual void SetRenderID(uint32_t) = 0;

    //==============================
    // in other APIs?
    virtual void SetUniformBlock(const std::string& name, uint32_t blockIndex) = 0;
};

class ShaderManager
{
public:
    static void Init();
    static void Add(const std::string& tag, const SharedPtr<Shader> shader);
    static SharedPtr<Shader> Create(const std::string& tag, const std::string& vertPath, const std::string& fragPath);
    static bool Exists(const std::string& tag);
    static SharedPtr<Shader> Get(const std::string& tag);
    static void ReloadShaders();

private:
    static std::unordered_map<std::string, SharedPtr<Shader>> shaderMap;
};

#endif // SHADER_H
