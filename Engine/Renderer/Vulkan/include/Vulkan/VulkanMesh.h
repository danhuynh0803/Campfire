#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Renderer/Texture.h"
#include "Vulkan/VulkanTexture.h"
#include "Vulkan/VulkanBuffer.h"
#include "Core/Base.h"
#include "Util/AABB.h"

#include "Core/ResourceManager.h"

class MaterialInstance;

namespace vk
{
    struct Vertex
    {
        glm::vec3 position;
        glm::vec2 texCoords;
        glm::vec3 normal;
        //glm::vec3 tangent;
        //glm::vec3 binormal;
    };

    struct VulkanSubmesh
    {
        VulkanSubmesh(std::vector<vk::Vertex> v, std::vector<uint32_t> i, std::vector<SharedPtr<Texture2D>> t)
            : vertices(v), indices(i), textures(t)
        {
            vertexBuffer = CreateSharedPtr<VulkanVertexBuffer>(vertices.data(), sizeof(vk::Vertex) * vertices.size());
            indexBuffer = CreateSharedPtr<VulkanIndexBuffer>(indices.data(), indices.size());
        }

        void UpdateDescriptors()
        {
            auto& descriptorSets = VulkanContext::Get()->GetPipeline()->descriptorSets;

            // TODO refactor this to be handled by materials
            if (textures.size() > 0)
            {
                auto texture = textures.at(0);

                SharedPtr<VulkanTexture2D> albedo = std::dynamic_pointer_cast<VulkanTexture2D>(
                    ResourceManager::GetTexture(albedoIndex)
                );

                SharedPtr<VulkanTexture2D> normal = std::dynamic_pointer_cast<VulkanTexture2D>(
                    ResourceManager::GetTexture(normalIndex)
                );

                for (int i = 0; i < 3; ++i)
                {
                    albedo->UpdateDescriptors(descriptorSets[i].get(), 2);
                    normal->UpdateDescriptors(descriptorSets[i].get(), 3);
                }
            }
        }

        uint32_t baseVertex;
        uint32_t baseIndex;
        uint32_t materialIndex;
        uint32_t indexCount;
        // TODO move to a material
        uint32_t albedoIndex;
        uint32_t normalIndex;

        glm::mat4 transform;
        std::string nodeName, meshName;
        SharedPtr<VulkanVertexBuffer> vertexBuffer;
        SharedPtr<VulkanIndexBuffer> indexBuffer;
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
        //std::vector<SharedPtr<VulkanTexture2D>> textures;
        std::vector<SharedPtr<Texture2D>> textures;
        AABB boundingBox;
    };

    class VulkanMesh
    {
    public:
        VulkanMesh(const std::string& filename);
        ~VulkanMesh();

        void OnUpdate(float dt);

        void Draw(vk::CommandBuffer commandBuffer);

        static SharedPtr<VulkanMesh> Create(const std::string& filename);

        //SharedPtr<VertexArray> GetVertexArray() { return vertexArray; }
        std::vector<VulkanSubmesh>& GetVulkanSubmeshes() { return submeshes; }
        SharedPtr<MaterialInstance> GetMaterialInstance(uint32_t index) { return materials.at(index); }
        std::string GetFilePath() { return filepath; }
        std::string GetName() const { return name; }
        void OnImGuiRender();

    private:
        void LoadModel(const std::string& path);
        void ProcessNode(aiNode* node, const aiScene* scene);
        VulkanSubmesh LoadSubmesh(aiMesh* mesh, const aiScene* scene);
        std::vector<SharedPtr<Texture2D>> LoadMaterialTextures(aiMaterial* mat, aiTextureType type);
    private:
        std::vector<VulkanSubmesh> submeshes;
        std::string filepath;
        std::string name;
        std::string directory;
        std::vector<SharedPtr<MaterialInstance>> materials;

    private: // assimp related
        //Assimp::Importer importer;
    };
}

