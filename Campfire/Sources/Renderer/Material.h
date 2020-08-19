#ifndef MATERIAL_H
#define MATERIAL_H

#include <string>

#include "Core/Base.h"
#include "Shader.h"
#include "Texture.h"

class Material
{
public:
    Material() = default;
    Material(const std::string& shaderPath);
    Material(const Shader& _shader);

    ~Material() = default;

    glm::vec3 albedo = glm::vec3(0.7f);

    SharedPtr<Shader> shader = nullptr;

    SharedPtr<Texture2D> albedoMap = nullptr;
    SharedPtr<Texture2D> specularMap = nullptr;
    SharedPtr<Texture2D> normalMap = nullptr;
    SharedPtr<Texture2D> roughnessMap = nullptr;
    SharedPtr<Texture2D> ambientOcclusionMap = nullptr;
    SharedPtr<Texture2D> opacityMap = nullptr;
};

#endif // MATERIAL_H
