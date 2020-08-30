#pragma once

#include "Core/Base.h"
#include "Core/FileSystem.h"
#include "Shader.h"
#include "Texture.h"

#include <string>

class Material
{
public:
    virtual void Bind() const = 0;
    virtual void OnImGuiRender() = 0;
};

class MaterialInstance : public Material
{
public:
    enum Type
    {
        PBR = 0,
        SPRITE,
        SPRITE_DIFFUSE
    };

    glm::vec4 color = glm::vec4(1.0f);

    static SharedPtr<MaterialInstance> Create(MaterialInstance::Type type);
    virtual SharedPtr<Shader> GetShader() const { return shader; }

protected:
    SharedPtr<Shader> shader = nullptr;
};

class PbrMaterial : public MaterialInstance
{
public:
    PbrMaterial();
    virtual void Bind() const override;
    virtual void OnImGuiRender() override;

    float metallic = 0.5f;
    float roughness = 0.5f;
    float ao = 0.5f;

    bool useAlbedoMap    = false;
    bool useMetallicMap  = false;
    bool useNormalMap    = false;
    bool useRoughnessMap = false;
    bool useOcclusionMap = false;

    SharedPtr<Texture2D> albedoMap = nullptr;
    SharedPtr<Texture2D> metallicMap = nullptr;
    SharedPtr<Texture2D> normalMap = nullptr;
    SharedPtr<Texture2D> roughnessMap = nullptr;
    SharedPtr<Texture2D> ambientOcclusionMap = nullptr;
    SharedPtr<Texture2D> opacityMap = nullptr;
};
