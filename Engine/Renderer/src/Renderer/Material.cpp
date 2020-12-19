#include "Renderer/Material.h"
#include <imgui.h>

SharedPtr<MaterialInstance> MaterialInstance::Create(MaterialInstance::Type type)
{
    switch (type)
    {
        case (MaterialInstance::Type::PBR):
        {
            return CreateSharedPtr<PbrMaterial>();
        }
        // TODO
        //case (Material::Instance::DIFFUSE):
        //{
        //    return CreateSharedPtr<DiffuseMaterial>();
        //}
        //case (Material::Instance::SPRITE):
        //{
        //    return CreateSharedPtr<SpriteMaterial>();
        //}
        //case (Material::Instance::SPRITE_DIFFUSE):
        //{
        //    return CreateSharedPtr<SpriteDiffuseMaterial>();
        //}
    }

    return nullptr;
}

PbrMaterial::PbrMaterial()
{
    shader = ShaderManager::Get("pbr");

    SharedPtr<Texture2D> whiteTexture = Texture2D::Create(1, 1);
    uint32_t whiteTextureData = 0xffffffff;
    whiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));

    SharedPtr<Texture2D> blackTexture = Texture2D::Create(1, 1);
    uint32_t blackTextureData = 0x00000000;
    whiteTexture->SetData(&blackTextureData, sizeof(uint32_t));

    albedoMap           = whiteTexture;
    metallicMap         = blackTexture;
    normalMap           = blackTexture;
    roughnessMap        = blackTexture;
    ambientOcclusionMap = blackTexture;
    emissiveMap         = blackTexture;
}

void PbrMaterial::Bind() const
{
    shader->Bind();

    shader->SetFloat4("uAlbedo", color);
    shader->SetFloat("uMetallic", metallic);
    shader->SetFloat("uRoughness", roughness);
    shader->SetFloat("uAO", ao);
    shader->SetFloat3("uEmissionColor", emissiveColor);
    shader->SetFloat("uEmissiveIntensity", emissiveIntensity);

    shader->SetBool("useAlbedoMap", useAlbedoMap);
    shader->SetBool("useMetallicMap", useMetallicMap);
    shader->SetBool("useNormalMap", useNormalMap);
    shader->SetBool("useRoughnessMap", useRoughnessMap);
    shader->SetBool("useOcclusionMap", useOcclusionMap);
    shader->SetBool("useEmissiveMap", useEmissiveMap);

    shader->SetInt("albedoMap", 0);
    shader->SetInt("metallicMap", 1);
    shader->SetInt("normalMap", 2);
    shader->SetInt("roughnessMap", 3);
    shader->SetInt("ambientOcclusionMap", 4);
    shader->SetInt("emissiveMap", 5);

    shader->SetInt("skybox", 6);

    if (albedoMap)
    {
        albedoMap->Bind(0);
    }

    if (metallicMap)
    {
        metallicMap->Bind(1);
    }

    if (normalMap)
    {
        normalMap->Bind(2);
    }

    if (roughnessMap)
    {
        roughnessMap->Bind(3);
    }

    if (ambientOcclusionMap)
    {
        ambientOcclusionMap->Bind(4);
    }

    if (emissiveMap)
    {
        emissiveMap->Bind(5);
    }
}

void PbrMaterial::OnImGuiRender()
{
    int id = 0;
    //==============================================
    // Albedo
    ImGui::PushID(id);
    ImGui::Checkbox("", &useAlbedoMap); ImGui::SameLine();
    id++;
    ImGui::PopID();
    if (ImGui::ImageButton((ImTextureID)albedoMap->GetRenderID(), ImVec2(16, 16), ImVec2(0,1), ImVec2(1,0), -1, ImVec4(0,0,0,0), ImVec4(0.9, 0.9f, 0.9f, 1.0f)))
    {
        std::string path = FileSystem::OpenFile();
        if (!path.empty())
        {
            albedoMap = Texture2D::Create(path);
        }
    }
    ImGui::SameLine(); ImGui::ColorEdit4("Albedo", (float*)&color);
    //==============================================

    //==============================================
    // Normals
    ImGui::PushID(id);
    ImGui::Checkbox("", &useNormalMap); ImGui::SameLine();
    id++;
    ImGui::PopID();
    if (ImGui::ImageButton((ImTextureID)normalMap->GetRenderID(), ImVec2(16, 16), ImVec2(0,1), ImVec2(1,0), -1, ImVec4(0,0,0,0), ImVec4(0.9, 0.9f, 0.9f, 1.0f)))
    {
        std::string path = FileSystem::OpenFile();
        if (!path.empty())
        {
            normalMap = Texture2D::Create(path);
        }
    }
    ImGui::SameLine();
    ImGui::Text("Normals");
    //==============================================

    //==============================================
    // Metallic
    ImGui::PushID(id);
    ImGui::Checkbox("", &useMetallicMap); ImGui::SameLine();
    id++;
    ImGui::PopID();
    if (ImGui::ImageButton((ImTextureID)metallicMap->GetRenderID(), ImVec2(16, 16), ImVec2(0,1), ImVec2(1,0), -1, ImVec4(0,0,0,0), ImVec4(0.9, 0.9f, 0.9f, 1.0f)))
    {
        std::string path = FileSystem::OpenFile();
        if (!path.empty())
        {
            metallicMap = Texture2D::Create(path);
        }
    }
    ImGui::SameLine();
    ImGui::SliderFloat("Metallic", &metallic, 0.0f, 1.0f);
    //==============================================

    //==============================================
    // Roughness
    ImGui::PushID(id);
    ImGui::Checkbox("", &useRoughnessMap); ImGui::SameLine();
    id++;
    ImGui::PopID();
    if (ImGui::ImageButton((ImTextureID)roughnessMap->GetRenderID(), ImVec2(16, 16), ImVec2(0,1), ImVec2(1,0), -1, ImVec4(0,0,0,0), ImVec4(0.9, 0.9f, 0.9f, 1.0f)))
    {
        std::string path = FileSystem::OpenFile();
        if (!path.empty())
        {
            roughnessMap = Texture2D::Create(path);
        }
    }
    ImGui::SameLine();
    ImGui::SliderFloat("Roughness", &roughness, 0.01f, 1.0f);
    //==============================================

    //==============================================
    // Occlusion
    ImGui::PushID(id);
    ImGui::Checkbox("", &useOcclusionMap); ImGui::SameLine();
    id++;
    ImGui::PopID();
    if (ImGui::ImageButton((ImTextureID)ambientOcclusionMap->GetRenderID(), ImVec2(16, 16), ImVec2(0,1), ImVec2(1,0), -1, ImVec4(0,0,0,0), ImVec4(0.9, 0.9f, 0.9f, 1.0f)))
    {
        std::string path = FileSystem::OpenFile();
        if (!path.empty())
        {
            ambientOcclusionMap = Texture2D::Create(path);
        }
    }
    ImGui::SameLine();
    ImGui::SliderFloat("Occlusion", &ao, 0.0f, 1.0f);
    //==============================================


    //==============================================
    // Emmission
    ImGui::PushID(id);
    ImGui::Checkbox("", &useEmissiveMap); ImGui::SameLine();
    id++;
    ImGui::PopID();
    if (ImGui::ImageButton((ImTextureID)emissiveMap->GetRenderID(), ImVec2(16, 16), ImVec2(0,1), ImVec2(1,0), -1, ImVec4(0,0,0,0), ImVec4(0.9, 0.9f, 0.9f, 1.0f)))
    {
        std::string path = FileSystem::OpenFile();
        if (!path.empty())
        {
            emissiveMap = Texture2D::Create(path);
        }
    }
    ImGui::SameLine();
    ImGui::ColorEdit3("Color", (float*)&emissiveColor);
    ImGui::SliderFloat("Intensity", &emissiveIntensity, 0.0f, 10.0f);
    //==============================================
}

