#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <vector>
#include "Core/Base.h"

class Texture
{
public:
    virtual ~Texture() = default;

    virtual uint32_t GetWidth() const = 0;
    virtual uint32_t GetHeight() const = 0;
    virtual void SetData(void* data, uint32_t size) = 0;
    virtual void Bind(uint32_t unit = 0) const = 0;
    virtual std::string GetName() const = 0;
};

class Texture2D : public Texture
{
public:
    virtual ~Texture2D() = default;

    static SharedPtr<Texture2D> Create(const std::string& path);
    static SharedPtr<Texture2D> Create(uint32_t width, uint32_t height);

    virtual uint32_t GetRenderID() const = 0;
};

class Texture3D : public Texture
{
public:
    // TODO
    //static SharedPtr<Texture3D> Create(const std::string& path);
};

class TextureCube : public Texture
{
public:
    virtual ~TextureCube() = default;

    //static SharedPtr<TextureCube> Create(const std::string& path); // For single cubemap files that contain list of separate texture paths
    static SharedPtr<TextureCube> Create(const std::vector<std::string>& pathList);
};

#endif // TEXTURE_H
