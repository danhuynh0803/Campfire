#pragma once

#include "Renderer/Text.h"
#include "Renderer/VertexArray.h"
#include "Renderer/Buffer.h"
#include "Renderer/Shader.h"

class OpenGLFont : public Font
{
public:
    OpenGLFont(const std::string& fontPath, uint32_t fontSize);
    ~OpenGLFont() = default;
    std::string GetPath() const override { return path; }
private:
    std::string path;
};

class OpenGLText : public Text
{
public:
    OpenGLText(const std::string& newText);
    virtual void Draw() override;
    SharedPtr<VertexArray> vertexArray;
    SharedPtr<VertexBuffer> vertexBuffer;
    SharedPtr<Shader> shader;
};
