#ifndef MATERIAL_H
#define MATERIAL_H

#include <string>

#include "Core/Base.h"
#include "Shader.h"
#include "Texture.h"

class Material
{
public:
    Material(const std::string& shaderPath);
    Material(const Shader& _shader);
    ~Material() = default;

    template <typename T>
    void SetValue(const std::string& uniform, T value);

    void SetTexture(const Texture& texture);

private:
    SharedPtr<Shader> shader;
};

#endif // MATERIAL_H
