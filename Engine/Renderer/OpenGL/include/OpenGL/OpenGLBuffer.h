#pragma once

#include <stdint.h>
#include "Renderer/Buffer.h"

class OpenGLVertexBuffer : public VertexBuffer
{
public:
    OpenGLVertexBuffer(uint32_t size);
    OpenGLVertexBuffer(float* vertices, uint32_t size);
    virtual ~OpenGLVertexBuffer();
    virtual void Bind() const override;
    virtual void Unbind() const override;
    virtual void SetData(void* data, uint32_t size) override;
    virtual void SetLayout(const BufferLayout& _layout) override { layout = _layout; }
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

class OpenGLUniformBuffer : public UniformBuffer
{
public:
    OpenGLUniformBuffer();
    virtual ~OpenGLUniformBuffer();

    virtual void Bind() const override;
    virtual void Unbind() const override;

    virtual void SetData(void* data, uint32_t offset, uint32_t size) override;
    virtual void SetLayout(const BufferLayout& _layout, uint32_t blockIndex, uint32_t count = 1) override;
    virtual void SetLayout(uint32_t blockIndex, uint32_t size) override;

private:
    uint32_t renderID;
    BufferLayout layout;
};
