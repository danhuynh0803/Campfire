#ifndef MESH_H
#define MESH_H

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

struct Vertex
{
    glm::vec3 position;
    glm::vec2 texCoords;
    glm::vec3 normal;
    //glm::vec3 tangent;
    //glm::vec3 binormal;
};

struct Submesh
{
    Submesh(std::vector<Vertex> v, std::vector<uint32_t> i, std::vector<SharedPtr<Texture>> t)
        : vertices(v), indices(i), textures(t)
    {
        // Load render data
        vertexArray = VertexArray::Create();
        vertexArray->Bind();

        SharedPtr<VertexBuffer> buffer = VertexBuffer::Create(vertices.data(), vertices.size() * sizeof(Vertex));
        buffer->Bind();

        BufferLayout layout =
        {
            { ShaderDataType::FLOAT3, "aPos" },
            { ShaderDataType::FLOAT2, "aUV" },
            { ShaderDataType::FLOAT3, "aNormal" }
        };
        buffer->SetLayout(layout);

        SharedPtr<IndexBuffer> indexBuffer = IndexBuffer::Create(indices.data(), indices.size());
        vertexArray->AddVertexBuffer(buffer);
        vertexArray->SetIndexBuffer(indexBuffer);

        buffer->Unbind();
        vertexArray->Unbind();
    }

    uint32_t baseVertex;
    uint32_t baseIndex;
    uint32_t materialIndex;
    uint32_t indexCount;

    glm::mat4 transform;

    std::string nodeName, meshName;

    SharedPtr<VertexArray> vertexArray;
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<SharedPtr<Texture>> textures;
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

    std::string GetFilePath() { return filepath; }
    std::string GetName() const { return name; }

private:
    void LoadModel(const std::string& path);
    void ProcessNode(aiNode* node, const aiScene* scene);
    Submesh LoadSubmesh(aiMesh* mesh, const aiScene* scene);
    std::vector<SharedPtr<Texture>> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName);
private:
    std::vector<Submesh> submeshes;
    std::string filepath;
    std::string name;
    std::string directory;

private: // assimp related
    //Assimp::Importer importer;
};

#endif // MESH_H
