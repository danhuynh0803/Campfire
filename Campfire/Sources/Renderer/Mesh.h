#ifndef MESH_H
#define MESH_H

#include <vector>
#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Core/Base.h"
#include "Renderer/Texture.h"

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec3 binormal;
    glm::vec2 texCoords;
};

struct Submesh
{
    Submesh(std::vector<Vertex> v, std::vector<uint32_t> i, std::vector<SharedPtr<Texture>> t)
        : vertices(v), indices(i), textures(t)
    {
    }

    uint32_t baseVertex;
    uint32_t baseIndex;
    uint32_t materialIndex;
    uint32_t indexCount;

    glm::mat4 transform;
    std::string nodeName, meshName;

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<SharedPtr<Texture>> textures;
};

class Mesh
{
public:
    Mesh(const std::string& filename);
    ~Mesh();

    void OnUpdate(float dt);

private:
    void LoadModel(const std::string& path);
    void ProcessNode(aiNode* node, const aiScene* scene);
    Submesh LoadSubmesh(aiMesh* mesh, const aiScene* scene);
    std::vector<SharedPtr<Texture>> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName);
private:
    std::vector<Submesh> submeshes;
    std::string filepath;
    std::string directory;
};

#endif // MESH_H
