#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Renderer/Texture.h"
#include "Renderer/Buffer.h"
#include "Renderer/VertexArray.h"
#include "Core/Base.h"
#include "Util/AABB.h"

class MaterialInstance;

struct Submesh
{
    struct Vertex
    {
        glm::vec3 position;
        glm::vec2 texCoords;
        glm::vec3 normal;
        //glm::vec3 tangent;
        //glm::vec3 binormal;
    };

    Submesh(std::vector<Vertex> v, std::vector<uint32_t> i, std::vector<SharedPtr<Texture2D>> t);

    uint32_t baseVertex;
    uint32_t baseIndex;
    uint32_t materialIndex;
    uint32_t indexCount;

    glm::mat4 transform;

    std::string nodeName, meshName;

    SharedPtr<VertexArray> vertexArray;
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<SharedPtr<Texture2D>> textures;
    AABB boundingBox;
};

class Mesh
{
public:
    Mesh(const std::string& filename);
    ~Mesh();

    void OnUpdate(float dt);

    static SharedPtr<Mesh> Create(const std::string& filename);

    //SharedPtr<VertexArray> GetVertexArray() { return vertexArray; }
    std::vector<Submesh>& GetSubmeshes() { return submeshes; }
    SharedPtr<MaterialInstance> GetMaterialInstance(uint32_t index) { return materials.at(index); }
    std::string GetFilePath() { return filepath; }
    std::string GetName() const { return name; }
    void OnImGuiRender();

private:
    void LoadModel(const std::string& path);
    void ProcessNode(aiNode* node, const aiScene* scene);
    Submesh LoadSubmesh(aiMesh* mesh, const aiScene* scene);
    std::vector<SharedPtr<Texture2D>> LoadMaterialTextures(aiMaterial* mat, aiTextureType type);
private:
    std::vector<Submesh> submeshes;
    std::string filepath;
    std::string name;
    std::string directory;
    std::vector<SharedPtr<MaterialInstance>> materials;

private: // assimp related
    //Assimp::Importer importer;
};
