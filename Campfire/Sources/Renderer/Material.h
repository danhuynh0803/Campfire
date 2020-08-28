#pragma once

#include "Core/Base.h"
#include "Core/FileSystem.h"
#include "Shader.h"
#include "Texture.h"

#include <string>

class Material
{
public:
    enum class Instance
    {
        PBR = 0,
        DIFFUSE,
        SPRITE,
        SPRITE_DIFFUSE
    };

public:
    //virtual Material();
    //virtual Material(const Shader& _shader);
    //virtual ~Material() = default;

    virtual void Bind() {}
    virtual void OnImGuiRender() {}
    static SharedPtr<Material> Create(Material::Instance type);

    virtual SharedPtr<Shader> GetShader() { return shader; }

protected:
    SharedPtr<Shader> shader = nullptr;
};

class PbrMaterial : public Material
{
public:
    PbrMaterial();
    virtual void Bind() override;
    virtual void OnImGuiRender() override;

    glm::vec4 albedo = glm::vec4(1.0f);
    float metallic = 0.0f;
    float roughness = 0.0f;
    float ao = 0.0f;

    SharedPtr<Texture2D> albedoMap = nullptr;
    SharedPtr<Texture2D> metallicMap = nullptr;
    SharedPtr<Texture2D> normalMap = nullptr;
    SharedPtr<Texture2D> roughnessMap = nullptr;
    SharedPtr<Texture2D> ambientOcclusionMap = nullptr;
    SharedPtr<Texture2D> opacityMap = nullptr;
};
