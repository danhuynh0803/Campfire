#ifndef SHADER_H
#define SHADER_H

#include <string>

#include <glm/glm.hpp>

class Shader
{
public:
    static Shader* Create(const std::string& filepath);
    static Shader* Create(const std::string& name, const std::string& vertPath, const std::string& fragPath);

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
};

#endif // SHADER_H
