#ifndef OPENGL_BUFFER_H
#define OPENGL_BUFFER_H

#include <stdint.h>
#include "Renderer/Buffer.h"

class OpenGLVertexBuffer : public VertexBuffer
{
public:
    OpenGLVertexBuffer(float* vertices, uint32_t size);
    virtual ~OpenGLVertexBuffer();
    virtual void Bind() const override;
    virtual void Unbind() const override;
    virtual void SetLayout(const BufferLayout& _layout) override;
    virtual const BufferLayout& GetLayout() const override { return layout; }

private:
    uint32_t renderID;
    BufferLayout layout;
};


class OpenGLIndexBuffer : public IndexBuffer
{
public:
    OpenGLIndexBuffer(uint32_t* indices, uint32_t _count);
    virtual ~OpenGLIndexBuffer();
    virtual void Bind() const override;
    virtual void Unbind() const override;
    virtual uint32_t GetCount() const override;

private:
    uint32_t renderID;
    uint32_t count;
};

#endif // OPENGL_BUFFER_H
