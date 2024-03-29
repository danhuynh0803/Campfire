#ifndef OPENGL_SHADER_H
#define OPENGL_SHADER_H

#include "Renderer/Shader.h"

class OpenGLShader : public Shader
{
public:
    OpenGLShader(const std::string& filepath);

    OpenGLShader(const std::string& name, const std::string& vertPath, const std::string& fragPath);

    ~OpenGLShader() = default;

    virtual void SetBool(const std::string& name, bool value) override;

    virtual void SetInt(const std::string& name, int value) override;
    virtual void SetIntArray(const std::string& name, int* values, uint32_t count) override;

    virtual void SetFloat(const std::string& name, float value) override;
    virtual void SetFloat3(const std::string& name, const glm::vec3& value) override;
    virtual void SetFloat4(const std::string& name, const glm::vec4& value) override;

    virtual void SetMat4(const std::string& name, const glm::mat4& value) override;

    virtual void Bind() override;
    virtual void Unbind() override;

    virtual const std::string& GetName() const override { return name; }
    virtual const std::string& GetVertexName() const override { return vertexName; }
    virtual const std::string& GetFragmentName() const override { return fragName; }

    virtual void SetUniformBlock(const std::string& name, uint32_t blockIndex) override;

    virtual uint32_t GetRenderID() const override { return renderID; }
    virtual void SetRenderID(uint32_t newRenderID) override { renderID = newRenderID; }

private:
    void Compile(const std::string& vertexSrc, const std::string& fragSrc);
    void CheckCompileErrors(uint32_t id, const ShaderType& type);
private:
    uint32_t renderID;
    std::string name;
    std::string vertexName;
    std::string fragName;
};

#endif // OPENGL_SHADER_H
