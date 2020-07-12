#ifndef OPENGL_RENDERER_API_H
#define OPENGL_RENDERER_API_H

#include "Renderer/RendererAPI.h"

class OpenGLRendererAPI : public RendererAPI
{
    virtual void Init() override;
    virtual void SetClearColor(const glm::vec4& color) override;
    virtual void Clear() override;

    virtual void DrawIndexed(const SharedPtr<VertexArray>& vertexArray) override;

};
#endif // OPENGL_RENDERER_API_H
