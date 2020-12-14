#pragma once

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

    static void SetDrawMode(const DrawMode& mode)
    {
        rendererAPI->SetDrawMode(mode);
    }

    static void DrawIndexed(const SharedPtr<VertexArray>& vertexArray)
    {
        rendererAPI->DrawIndexed(vertexArray);
    }

    static void DrawIndexedLines(const SharedPtr<VertexArray>& vertexArray)
    {
        rendererAPI->DrawIndexedLines(vertexArray);
    }

    static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
    {
        rendererAPI->SetViewport(x, y, width, height);
    }

private:
    static UniquePtr<RendererAPI> rendererAPI;
};
