#include "Renderer/Mesh.h"
#include "Core/ResourceManager.h"

std::vector<SharedPtr<Texture>> textureCache;

SharedPtr<Mesh> Mesh::Create(const std::string& filename)
{
    //return CreateSharedPtr<Mesh>(filename);
    //return ResourceManager::GetMesh(filename);
    return nullptr;
}

Mesh::Mesh(const std::string& filename)
    : filepath(filename)
{
    LoadModel(filename);
}

Mesh::~Mesh()
{
}

void Mesh::OnUpdate(float dt)
{
    // TODO for animations
}

void Mesh::LoadModel(const std::string& path)
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
        LOG_ERROR("ERROR::ASSIMP::{0}", importer.GetErrorString());
        return;
    }

    directory = path.substr(0, path.find_last_of('/'));
    name = path.substr(path.find_last_of('/') + 1);
    ProcessNode(scene->mRootNode, scene);
}

void Mesh::ProcessNode(aiNode* node, const aiScene* scene)
{
    // Process all the node's meshes (if any)
    for (unsigned int i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        submeshes.push_back(LoadSubmesh(mesh, scene));
    }

    // Then do the same for each of its children
    for (unsigned int i = 0; i < node->mNumChildren; ++i)
    {
        ProcessNode(node->mChildren[i], scene);
    }
}

Submesh Mesh::LoadSubmesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<SharedPtr<Texture>> textures;

    // Process vertex info
    for (size_t i = 0; i < mesh->mNumVertices; ++i)
    {
        Vertex vertex;

        // Position
        glm::vec3 tempVector;
        tempVector.x = mesh->mVertices[i].x;
        tempVector.y = mesh->mVertices[i].y;
        tempVector.z = mesh->mVertices[i].z;
        vertex.position = tempVector;

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

    if (mesh->mMaterialIndex >= 0)
    {
        // TODO check if there exists Albedo, specular, normal, bump maps, etc
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<SharedPtr<Texture>> diffuseMaps = LoadMaterialTextures(material,
                aiTextureType_DIFFUSE, "texture_diffuse");

        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        std::vector<SharedPtr<Texture>> specularMaps = LoadMaterialTextures(
                material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

        std::vector<SharedPtr<Texture>> normalMaps = LoadMaterialTextures(
                material, aiTextureType_NORMALS, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    }
    else
    {
        LOG_WARN("No Textures associated with {0}", name);
    }

    return Submesh(vertices, indices, textures);
}

std::vector<SharedPtr<Texture>> Mesh::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName)
{
    std::vector<SharedPtr<Texture>> textures;

    SharedPtr<Texture> texture;
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
            LOG_INFO("Loading Texture from {0}", path);
            texture = Texture2D::Create(path.c_str());
            textureCache.push_back(texture);
        }

        textures.push_back(texture);
    }

    return textures;
}
