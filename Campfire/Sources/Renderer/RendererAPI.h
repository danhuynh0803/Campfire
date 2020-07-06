#ifndef RENDERER_API_H
#define RENDERER_API_H

#include <glm/glm.hpp>

#include "VertexArray.h"

class RendererAPI
{
public:
    enum class API
    {
        None = 0,
        OpenGL
    };

public:
    virtual void Init() = 0;
    virtual void SetClearColor(const glm::vec4& color) = 0;
    virtual void Clear() = 0;

    virtual void DrawIndexed(const SharedPtr<VertexArray>& vertexArray) = 0;
    inline static API GetAPI() { return sAPI; }

    static UniquePtr<RendererAPI> Create();

private:
    static API sAPI;
};


#endif // RENDERER_API_H
