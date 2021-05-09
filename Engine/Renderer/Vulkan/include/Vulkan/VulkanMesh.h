#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <vulkan/vulkan.hpp>

#include "Core/Base.h"
#include "Util/AABB.h"

struct VulkanMaterial;
class VulkanVertexBuffer;
class VulkanIndexBuffer;
class MaterialInstance;
class Texture2D;

struct VulkanSubmesh
{
    struct Vertex
    {
        glm::vec3 position;
        glm::vec2 texCoords;
        glm::vec3 normal;
        //glm::vec3 tangent;
        //glm::vec3 binormal;
    };

    VulkanSubmesh(std::vector<Vertex> v, std::vector<uint32_t> i, SharedPtr<VulkanMaterial> m);

    uint32_t baseVertex;
    uint32_t baseIndex;
    uint32_t materialIndex;
    uint32_t indexCount;

    glm::mat4 transform;
    std::string nodeName, meshName;
    SharedPtr<VulkanVertexBuffer> vertexBuffer;
    SharedPtr<VulkanIndexBuffer> indexBuffer;
    SharedPtr<VulkanMaterial> material;
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    AABB boundingBox;
};

class VulkanMesh
{
public:
    VulkanMesh(const std::string& filename);
    ~VulkanMesh();

    void OnUpdate(float dt);

    void Draw(vk::CommandBuffer commandBuffer, uint32_t frameIndex);

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
