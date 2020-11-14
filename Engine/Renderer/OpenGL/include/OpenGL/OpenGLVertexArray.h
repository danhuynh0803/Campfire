#ifndef OPENGL_VERTEX_ARRAY_H
#define OPENGL_VERTEX_ARRAY_H

#include "Renderer/VertexArray.h"
#include "Renderer/Buffer.h"

class OpenGLVertexArray : public VertexArray
{
public:
    OpenGLVertexArray();
    ~OpenGLVertexArray();

    virtual void Bind() const override;
    virtual void Unbind() const override;

    virtual void AddVertexBuffer(const SharedPtr<VertexBuffer>& vertexBuffer) override;
    virtual void SetIndexBuffer(const SharedPtr<IndexBuffer>& indexBuffer) override;

    virtual const SharedPtr<IndexBuffer>& GetIndexBuffer() const override
    {
        return indexBuffer;
    }

private:
    uint32_t renderID;
    std::vector<std::shared_ptr<VertexBuffer>> vertexBuffers;
    std::shared_ptr<IndexBuffer> indexBuffer;
};

#endif // OPENGL_VERTEX_ARRAY_H
