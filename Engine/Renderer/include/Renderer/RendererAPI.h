#pragma once

#include <glm/glm.hpp>

#include "VertexArray.h"

enum class DrawMode
{
    SHADED = 0,
    WIREFRAME,
};

class RendererAPI
{
public:
    enum class API
    {
        None = 0,
        OpenGL,
        Vulkan,
    };

public:
    virtual void Init() = 0;
    virtual void SetClearColor(const glm::vec4& color) = 0;
    virtual void Clear() = 0;

    virtual void SetDrawMode(const DrawMode& mode) = 0;
    virtual void DrawIndexed(const SharedPtr<VertexArray>& vertexArray) = 0;
    virtual void DrawIndexedLines(const SharedPtr<VertexArray>& vertexArray) = 0;
    virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
    inline static API GetAPI() { return sAPI; }

    static UniquePtr<RendererAPI> Create();

private:
    static API sAPI;
};
