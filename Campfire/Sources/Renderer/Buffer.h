#ifndef BUFFER_H
#define BUFFER_H

#include <stdint.h>

struct VertexBuffer
{
    virtual ~VertexBuffer() {}
    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;

    static VertexBuffer* Create(float* vertices, uint32_t size);
};

struct IndexBuffer
{
    virtual ~IndexBuffer() {}
    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;
    virtual uint32_t GetCount() const = 0;

    static IndexBuffer* Create(uint32_t* indices, uint32_t _count);
};

#endif // BUFFER_H
