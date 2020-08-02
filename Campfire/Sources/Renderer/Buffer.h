#ifndef BUFFER_H
#define BUFFER_H

#include <cstdint>
#include <string>
#include <vector>

#include "Core/Base.h"

//================================================
//----------------Buffer Layout-------------------
//================================================
enum class ShaderDataType
{
    NONE = 0,
    INT, INT2, INT3, INT4,
    FLOAT, FLOAT2, FLOAT3, FLOAT4,
    MAT3, MAT4,
    BOOL
};

static uint32_t ConvertShaderDataTypeToSize(ShaderDataType type)
{
    switch (type)
    {
        case ShaderDataType::INT:       return     sizeof(int);
        case ShaderDataType::INT2:      return 2 * sizeof(int);
        case ShaderDataType::INT3:      return 3 * sizeof(int);
        case ShaderDataType::INT4:      return 4 * sizeof(int);

        case ShaderDataType::FLOAT:     return     sizeof(float);
        case ShaderDataType::FLOAT2:    return 2 * sizeof(float);
        case ShaderDataType::FLOAT3:    return 3 * sizeof(float);
        case ShaderDataType::FLOAT4:    return 4 * sizeof(float);

        case ShaderDataType::MAT3:      return 9  * sizeof(float);
        case ShaderDataType::MAT4:      return 16 * sizeof(float);

        case ShaderDataType::BOOL:      return sizeof(bool);
    }

    return 0;
}

static uint32_t ConvertShaderDataTypeToCount(ShaderDataType type)
{
    switch (type)
    {
        case ShaderDataType::INT:       return 1;
        case ShaderDataType::INT2:      return 2;
        case ShaderDataType::INT3:      return 3;
        case ShaderDataType::INT4:      return 4;

        case ShaderDataType::FLOAT:     return 1;
        case ShaderDataType::FLOAT2:    return 2;
        case ShaderDataType::FLOAT3:    return 3;
        case ShaderDataType::FLOAT4:    return 4;

        case ShaderDataType::MAT3:      return 9;
        case ShaderDataType::MAT4:      return 16;

        case ShaderDataType::BOOL:      return 1;
    }
}

struct BufferElement
{
    BufferElement(ShaderDataType _type, const std::string& _name, bool isNormalized = false)
        : type(_type),
          name(_name),
          componentCount(ConvertShaderDataTypeToCount(type)),
          size(ConvertShaderDataTypeToSize(type)),
          normalized(isNormalized),
          offset(0)
    {
    }

    ShaderDataType type;
    std::string name;
    uint32_t componentCount;
    bool normalized;
    uint32_t size;
    uint32_t offset;
};

class BufferLayout
{
public:
    BufferLayout() = default;
    BufferLayout(const std::initializer_list<BufferElement>& element)
        : elements(element)
    {
        CalculateOffsetAndStride();
    }

    const uint32_t GetStride() const { return stride; }

    const std::vector<BufferElement>& GetElements() const { return elements; }

    std::vector<BufferElement>::iterator begin() { return elements.begin(); }
    std::vector<BufferElement>::iterator end()   { return elements.end(); }
    std::vector<BufferElement>::const_iterator begin() const { return elements.begin(); }
    std::vector<BufferElement>::const_iterator end()   const { return elements.end(); }

private:
    void CalculateOffsetAndStride()
    {
        int offset = 0;
        for (auto& element : elements)
        {
            element.offset = offset;
            offset += element.size;
            stride += element.size;
        }
    }
private:
    uint32_t stride = 0;
    std::vector<BufferElement> elements;
};


//================================================
//------------------Buffers-----------------------
//================================================
struct VertexBuffer
{
    virtual ~VertexBuffer() {}
    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;
    virtual void SetData(void* data, uint32_t size) = 0;
    virtual void SetLayout(const BufferLayout& layout) = 0;
    virtual const BufferLayout& GetLayout() const = 0;

    static SharedPtr<VertexBuffer> Create(uint32_t size);
    static SharedPtr<VertexBuffer> Create(void* vertices, uint32_t size);
};

struct IndexBuffer
{
    virtual ~IndexBuffer() {}
    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;
    virtual uint32_t GetCount() const = 0;

    static SharedPtr<IndexBuffer> Create(uint32_t* indices, uint32_t _count);
};

struct UniformBuffer
{
    virtual ~UniformBuffer() {}
    virtual void Bind() const = 0;
    virtual void Unbind() const = 0;
    virtual void SetData(void* data, uint32_t offset, uint32_t size) = 0;
    virtual void SetLayout(const BufferLayout& layout, uint32_t blockIndex) = 0;

    static SharedPtr<UniformBuffer> Create();
};

#endif // BUFFER_H
