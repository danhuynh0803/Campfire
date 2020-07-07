#ifndef RENDER_COMMAND_H
#define RENDER_COMMAND_H

#include "Renderer/RendererAPI.h"

class RenderCommand
{
public:
    static void Init()
    {
        rendererAPI->Init();
    }

    static void SetClearColor(const glm::vec4& color)
    {
        rendererAPI->SetClearColor(color);
    }

    static void Clear()
    {
        rendererAPI->Clear();
    }

    static void DrawIndexed(const SharedPtr<VertexArray>& vertexArray)
    {
        rendererAPI->DrawIndexed(vertexArray);
    }

private:
    static UniquePtr<RendererAPI> rendererAPI;
};

#endif // RENDER_COMMAND_H
