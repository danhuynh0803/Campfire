#include "Vulkan/VulkanMesh.h"
#include "Core/ResourceManager.h"
#include "Util/AABB.h"

#include "Vulkan/VulkanRenderer.h"
#include "Vulkan/VulkanTexture.h"
#include "Vulkan/VulkanContext.h"
#include "Vulkan/VulkanMesh.h"
#include "Vulkan/VulkanMaterial.h"
#include "Vulkan/VulkanInitializers.h"
#include "Vulkan/VulkanMaterial.h"
#include "Vulkan/VulkanPipeline.h"
#include "Vulkan/VulkanBuffer.h"

#include "Renderer/Material.h"
#include "Renderer/FrameGraph.h"

#include <glm/glm.hpp>

namespace
{
    std::vector<SharedPtr<Texture2D>> textureCache;

    struct TextureMapUsage
    {
        bool useAlbedoMap    = false;
        bool useMetallicMap  = false;
        bool useNormalMap    = false;
        bool useRoughnessMap = false;
        bool useOcclusionMap = false;
        bool useEmissiveMap  = false;
    };
}

VulkanSubmesh::VulkanSubmesh(std::vector<Vertex> v, std::vector<uint32_t> i, SharedPtr<VulkanMaterial> m)
    : vertices(v), indices(i), material(m)
{
    vertexBuffer = CreateSharedPtr<VulkanVertexBuffer>(vertices.data(), sizeof(Vertex) * vertices.size());
    indexBuffer = CreateSharedPtr<VulkanIndexBuffer>(indices.data(), indices.size());


    // TODO handled by material
    std::vector<vk::DescriptorSetLayoutBinding> descriptorSet{};
    { // Material descriptors
        // Albedo map
        auto albedo = vk::initializers::DescriptorSetLayoutBinding(
            vk::DescriptorType::eCombinedImageSampler,
            vk::ShaderStageFlagBits::eFragment,
            0);

        // Metallic map
        auto metallic = vk::initializers::DescriptorSetLayoutBinding(
            vk::DescriptorType::eCombinedImageSampler,
            vk::ShaderStageFlagBits::eFragment,
            1);

        // Normal map
        auto normal = vk::initializers::DescriptorSetLayoutBinding(
            vk::DescriptorType::eCombinedImageSampler,
            vk::ShaderStageFlagBits::eFragment,
            2);

        // Roughness map
        auto roughness = vk::initializers::DescriptorSetLayoutBinding(
            vk::DescriptorType::eCombinedImageSampler,
            vk::ShaderStageFlagBits::eFragment,
            3);

        // Ambient Occlusion map
        auto ambientOcclusion = vk::initializers::DescriptorSetLayoutBinding(
            vk::DescriptorType::eCombinedImageSampler,
            vk::ShaderStageFlagBits::eFragment,
            4);

        // Emissive map
        auto emissive = vk::initializers::DescriptorSetLayoutBinding(
            vk::DescriptorType::eCombinedImageSampler,
            vk::ShaderStageFlagBits::eFragment,
            5);

        // TextureMap usage
        auto mapUsage = vk::initializers::DescriptorSetLayoutBinding(
            vk::DescriptorType::eUniformBuffer,
            vk::ShaderStageFlagBits::eFragment,
            6);

        descriptorSet = {
            albedo,
            metallic,
            normal,
            roughness,
            ambientOcclusion,
            emissive,
            mapUsage,
        };
    }

    auto descriptorSetLayoutInfo = vk::initializers::DescriptorSetLayoutCreateInfo(
        descriptorSet.size(),
        descriptorSet.data()
    );

    auto device = VulkanContext::Get()->GetDevice()->GetVulkanDevice();
    material->mDescriptorSetLayouts.emplace_back(device.createDescriptorSetLayoutUnique(descriptorSetLayoutInfo));

    auto allocInfo = vk::initializers::
        DescriptorSetAllocateInfo(
            VulkanContext::Get()->GetDescriptorPool(),
            material->mDescriptorSetLayouts.size(),
            &material->mDescriptorSetLayouts[0].get()
        );
    material->mDescriptorSets = device.allocateDescriptorSetsUnique(allocInfo);

    if (material)
    {
        SharedPtr<VulkanTexture2D> albedo = std::static_pointer_cast<VulkanTexture2D>(
            material->albedoMap
        );

        SharedPtr<VulkanTexture2D> metallic = std::static_pointer_cast<VulkanTexture2D>(
            material->metallicMap
        );

        SharedPtr<VulkanTexture2D> normal = std::static_pointer_cast<VulkanTexture2D>(
            material->normalMap
        );

        SharedPtr<VulkanTexture2D> roughness = std::static_pointer_cast<VulkanTexture2D>(
            material->roughnessMap
        );

        SharedPtr<VulkanTexture2D> occlusion = std::static_pointer_cast<VulkanTexture2D>(
            material->occlusionMap
        );

        SharedPtr<VulkanTexture2D> emissive = std::static_pointer_cast<VulkanTexture2D>(
            material->emissiveMap
        );

        material->textureMapUsageUbo = CreateSharedPtr<VulkanUniformBuffer>(
            sizeof(::TextureMapUsage)
        );

        BufferLayout usageLayout =
        {
            { ShaderDataType::BOOL, "useAlbedoMap"    },
            { ShaderDataType::BOOL, "useMetallicMap"  },
            { ShaderDataType::BOOL, "useNormalMap"    },
            { ShaderDataType::BOOL, "useRoughnessMap" },
            { ShaderDataType::BOOL, "useOcclusionMap" },
            { ShaderDataType::BOOL, "useEmissiveMap"  },
        };

        TextureMapUsage usage;

        if (albedo) {
            albedo->UpdateDescriptors(material->mDescriptorSets[0].get(), 0);
            usage.useAlbedoMap = true;
        }
        else { albedo->UpdateDescriptors(material->mDescriptorSets[0].get(), 0); }

        if (metallic) {
            metallic->UpdateDescriptors(material->mDescriptorSets[0].get(), 1);
            usage.useMetallicMap = true;
        }
        else { albedo->UpdateDescriptors(material->mDescriptorSets[0].get(), 1); }

        if (normal) {
            normal->UpdateDescriptors(material->mDescriptorSets[0].get(), 2);
            usage.useNormalMap = true;
        }
        else { albedo->UpdateDescriptors(material->mDescriptorSets[0].get(), 2); }

        if (roughness) {
            roughness->UpdateDescriptors(material->mDescriptorSets[0].get(), 3);
            usage.useRoughnessMap = true;
        }
        else { albedo->UpdateDescriptors(material->mDescriptorSets[0].get(), 3); }

        if (occlusion) {
            occlusion->UpdateDescriptors(material->mDescriptorSets[0].get(), 4);
            usage.useOcclusionMap = true;
        }
        else { albedo->UpdateDescriptors(material->mDescriptorSets[0].get(), 4); }

        if (emissive) {
            emissive->UpdateDescriptors(material->mDescriptorSets[0].get(), 5);
            usage.useEmissiveMap = true;
        }
        else { albedo->UpdateDescriptors(material->mDescriptorSets[0].get(), 5); }

        material->textureMapUsageUbo->UpdateDescriptorSet(
            material->mDescriptorSets[0], usageLayout, 6
        );
        material->textureMapUsageUbo->SetData(&usage, 6, sizeof(::TextureMapUsage));
    }
}

SharedPtr<VulkanMesh> VulkanMesh::Create(const std::string& filename)
{
    // TODO refactor to work with platform agnostic wrappers
    //return ResourceManager::Mesh(filename);
    return nullptr;
}

VulkanMesh::VulkanMesh(const std::string& filename)
    : filepath(filename)
{
    LoadModel(filename);
}

VulkanMesh::~VulkanMesh()
{
}

void VulkanMesh::OnUpdate(float dt)
{
    // TODO for animations
}

void VulkanMesh::Draw(vk::CommandBuffer commandBuffer, vk::PipelineLayout pipelineLayout)
{
    // TODO move to meshRenderer
    auto device = VulkanContext::Get()->GetDevice()->GetVulkanDevice();
    //auto pipeline = VulkanContext::Get()->mFrameGraph->GetGraphicsPipeline("models");
    for (auto& submesh : submeshes)
    {
        // Bind descriptor of material
        commandBuffer.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            pipelineLayout,
            1,
            1,
            &submesh.material->mDescriptorSets[0].get(),
            0, nullptr
        );

        VulkanRenderer::DrawIndexed(
            commandBuffer,
            submesh.vertexBuffer->GetBuffer(),
            submesh.indexBuffer->GetBuffer(),
            submesh.indices.size()
        );
    }
}

void VulkanMesh::LoadModel(const std::string& path)
{
    Assimp::Importer importer;

    const uint32_t meshImportFlags =
        aiProcess_Triangulate
        | aiProcess_GenUVCoords
        | aiProcess_GenNormals
        | aiProcess_OptimizeMeshes
        | aiProcess_ValidateDataStructure
        //| aiProcess_FlipUVs
        ;

    const aiScene* scene = importer.ReadFile(path, meshImportFlags);

    if (!scene || !scene->HasMeshes())
    {
        CORE_ERROR("ERROR::ASSIMP::{0}", importer.GetErrorString());
        return;
    }

    directory = path.substr(0, path.find_last_of('/'));
    name = path.substr(path.find_last_of('/') + 1);
    ProcessNode(scene->mRootNode, scene);
}

void VulkanMesh::ProcessNode(aiNode* node, const aiScene* scene)
{
    // Process all the node's meshes (if any)
    for (unsigned int i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        submeshes.push_back(LoadSubmesh(mesh, scene));
    }

    // Then do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; ++i)
    {
        ProcessNode(node->mChildren[i], scene);
    }
}

VulkanSubmesh VulkanMesh::LoadSubmesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<VulkanSubmesh::Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<SharedPtr<Texture2D>> submeshTextures;
    AABB aabb;
    aabb.mMin = { FLT_MAX, FLT_MAX, FLT_MAX };
    aabb.mMax = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

    uint32_t albedoIndex = 0;
    uint32_t normalIndex = 0;

    SharedPtr<VulkanMaterial> mat = CreateSharedPtr<VulkanMaterial>();

    // Process vertex info
    for (size_t i = 0; i < mesh->mNumVertices; ++i)
    {
        VulkanSubmesh::Vertex vertex;

        // Position
        glm::vec3 tempVector;
        tempVector.x = mesh->mVertices[i].x;
        tempVector.y = mesh->mVertices[i].y;
        tempVector.z = mesh->mVertices[i].z;
        vertex.position = tempVector;

        // Update AABB
        aabb.mMin.x = glm::min(vertex.position.x, aabb.mMin.x);
        aabb.mMin.y = glm::min(vertex.position.y, aabb.mMin.y);
        aabb.mMin.z = glm::min(vertex.position.z, aabb.mMin.z);
        aabb.mMax.x = glm::max(vertex.position.x, aabb.mMax.x);
        aabb.mMax.y = glm::max(vertex.position.y, aabb.mMax.y);
        aabb.mMax.z = glm::max(vertex.position.z, aabb.mMax.z);

        // Normals
        tempVector.x = mesh->mNormals[i].x;
        tempVector.y = mesh->mNormals[i].y;
        tempVector.z = mesh->mNormals[i].z;
        vertex.normal = tempVector;

        // TexCoords
        if (mesh->mTextureCoords[0])
        {
            glm::vec2 tex;
            tex.x = mesh->mTextureCoords[0][i].x;
            tex.y = mesh->mTextureCoords[0][i].y;
            vertex.texCoords = tex;
        }
        else
        {
            vertex.texCoords = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    // Process indices
    for (size_t i = 0; i < mesh->mNumFaces; ++i)
    {
        aiFace face = mesh->mFaces[i];
        for (size_t j = 0; j < face.mNumIndices; ++j)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    // TODO default to pbr as the standard material
    //SharedPtr<MaterialInstance> material = MaterialInstance::Create(MaterialInstance::Type::PBR);
    //material->name = mesh->mName.C_Str();
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* meshMaterial = scene->mMaterials[mesh->mMaterialIndex];

        // TODO check what type of material it is, to assign the proper material instance
        // Default to use pbr material for now;
        //if (mesh->type)

        //auto mat = std::dynamic_pointer_cast<PbrMaterial>(material);

        { // Ambient

        }

        { // Diffuse/Albedo
            std::vector<SharedPtr<Texture2D>> textures =
                LoadMaterialTextures(meshMaterial, aiTextureType_DIFFUSE);
            if (textures.size() > 0)
            {
                submeshTextures.emplace_back(textures.at(0));
                mat->albedoMap = textures.at(0);
                mat->useAlbedoMap = true;
            }
        }

        { // Displacement

        }

        { // Emmissive
            std::vector<SharedPtr<Texture2D>> textures =
                LoadMaterialTextures(meshMaterial, aiTextureType_EMISSIVE);
            if (textures.size() > 0)
            {
                mat->emissiveMap = textures.at(0);
                mat->useEmissiveMap = true;
            }
        }

        { // Height

        }

        { // Lightmap (Ambient Occlusion)
            std::vector<SharedPtr<Texture2D>> textures =
                LoadMaterialTextures(meshMaterial, aiTextureType_LIGHTMAP);
            if (textures.size() > 0)
            {
                mat->occlusionMap = textures.at(0);
                mat->useOcclusionMap = true;
            }
        }

        { // Normal
            std::vector<SharedPtr<Texture2D>> textures =
                LoadMaterialTextures(meshMaterial, aiTextureType_NORMALS);
            if (textures.size() > 0)
            {
                submeshTextures.emplace_back(textures.at(0));
                mat->normalMap = textures.at(0);
                mat->useNormalMap = true;
            }
        }

        { // Opacity

        }

        //{ // Reflection
        //    std::vector<SharedPtr<Texture2D>> textures =
        //        LoadMaterialTextures(meshMaterial, aiTextureType_REFLECTION);
        //    if (textures.size() > 0)
        //    {
        //    }
        //}

        { // Shininess

        }

        //{ // Specular
        //    std::vector<SharedPtr<Texture2D>> textures =
        //        LoadMaterialTextures(meshMaterial, aiTextureType_SPECULAR);
        //}
    }
    else
    {
        //CORE_WARN("No Textures associated with {0}", name);
    }

    //materials.emplace_back(material);
    VulkanSubmesh submesh(vertices, indices, mat);
    submesh.boundingBox = aabb;

    return submesh;
}

std::vector<SharedPtr<Texture2D>> VulkanMesh::LoadMaterialTextures(aiMaterial* mat, aiTextureType type)
{
    std::vector<SharedPtr<Texture2D>> textures;

    SharedPtr<Texture2D> texture;
    for (size_t i = 0; i < mat->GetTextureCount(type); ++i)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        std::string path(directory + '/' + str.C_Str());

        bool inCache = false;
        for (auto tex : textureCache)
        {
            if (path.compare(tex->GetName()) == 0)
            {
                inCache = true;
                texture = tex;
                break;
            }
        }
        if (!inCache)
        {
            CORE_INFO("Loading Texture from {0}", path);
            texture = Texture2D::Create(path.c_str());
            textureCache.push_back(texture);
        }

        textures.push_back(texture);
    }

    return textures;
}

void VulkanMesh::OnImGuiRender()
{
    //    ImGui::Text(name.c_str());
    //
    //    ImGui::Separator();
    //
    //    if (ImGui::TreeNode("Material List"))
    //    {
    //        for (int i = 0; i < materials.size(); ++i)
    //        {
    //            ImGui::PushID(i);
    //            auto& material = materials.at(i);
    //
    //            if (ImGui::TreeNode(material->name.c_str()))
    //            {
    //                material->OnImGuiRender();
    //                ImGui::TreePop();
    //            }
    //            ImGui::Separator();
    //
    //            ImGui::PopID();
    //        }
    //        ImGui::TreePop();
    //    }
}
