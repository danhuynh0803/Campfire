#ifndef VERTEX_ARRAY_H
#define VERTEX_ARRAY_H

#include "Renderer/Buffer.h"
#include "Core/Base.h"

class VertexArray
{
public:
    virtual ~VertexArray() {}

    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;

    virtual void AddVertexBuffer(const SharedPtr<VertexBuffer>& vertexBuffer) = 0;
    virtual void SetIndexBuffer(const SharedPtr<IndexBuffer>& indexBuffer) = 0;

    static SharedPtr<VertexArray> Create();
};

#endif // VERTEX_ARRAY_H
