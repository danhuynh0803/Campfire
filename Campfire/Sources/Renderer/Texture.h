#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include "Core/Base.h"

class Texture
{
public:
    virtual ~Texture() = default;

    virtual uint32_t GetWidth() const = 0;
    virtual uint32_t GetHeight() const = 0;

    virtual void Bind(uint32_t unit = 0) const = 0;

    static SharedPtr<Texture> Create(const std::string& path);
};

#endif // TEXTURE_H
