#include "Renderer/Mesh.h"
#include "Core/ResourceManager.h"
#include "Util/AABB.h"
#include "Renderer/Material.h"

std::vector<SharedPtr<Texture2D>> textureCache;

SharedPtr<Mesh> Mesh::Create(const std::string& filename)
{
    return ResourceManager::GetMesh(filename);
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
        CORE_ERROR("ERROR::ASSIMP::{0}", importer.GetErrorString());
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
    std::vector<SharedPtr<Texture2D>> textures;
    AABB aabb;
    aabb.mMin = { FLT_MAX, FLT_MAX, FLT_MAX };
    aabb.mMax = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

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

        // Update AABB bounds
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
    SharedPtr<MaterialInstance> material = MaterialInstance::Create(MaterialInstance::Type::PBR);
    material->name = mesh->mName.C_Str();
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* meshMaterial = scene->mMaterials[mesh->mMaterialIndex];

        // TODO check what type of material it is, to assign the proper material instance
        // Default to use pbr material for now;
        //if (mesh->type)

        auto mat = std::dynamic_pointer_cast<PbrMaterial>(material);

        { // Ambient

        }

        { // Diffuse
            std::vector<SharedPtr<Texture2D>> textures =
                LoadMaterialTextures(meshMaterial, aiTextureType_DIFFUSE);
            if (textures.size() > 0)
            {
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
                mat->ambientOcclusionMap = textures.at(0);
                mat->useOcclusionMap = true;
            }
        }

        { // Normal
            std::vector<SharedPtr<Texture2D>> textures =
                LoadMaterialTextures(meshMaterial, aiTextureType_NORMALS);
            if (textures.size() > 0)
            {
                mat->normalMap = textures.at(0);
                mat->useNormalMap = true;
            }
        }

        { // Opacity

        }

        { // Reflection
            std::vector<SharedPtr<Texture2D>> textures =
                LoadMaterialTextures(meshMaterial, aiTextureType_REFLECTION);
            if (textures.size() > 0)
            {
            }
        }

        { // Shininess

        }

        { // Specular
            std::vector<SharedPtr<Texture2D>> textures =
                LoadMaterialTextures(meshMaterial, aiTextureType_SPECULAR);
        }
    }
    else
    {
        CORE_WARN("No Textures associated with {0}", name);
    }

    materials.emplace_back(material);
    Submesh submesh(vertices, indices, textures);
    submesh.boundingBox = aabb;

    return submesh;
}

std::vector<SharedPtr<Texture2D>> Mesh::LoadMaterialTextures(aiMaterial* mat, aiTextureType type)
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

void Mesh::OnImGuiRender()
{
    ImGui::Text(name.c_str());

    ImGui::Separator();

    if (ImGui::TreeNode("Material List"))
    {
        for (int i = 0; i < materials.size(); ++i)
        {
            ImGui::PushID(i);
            auto& material = materials.at(i);

            if (ImGui::TreeNode(material->name.c_str()))
            {
                material->OnImGuiRender();
                ImGui::TreePop();
            }
            ImGui::Separator();

            ImGui::PopID();
        }
        ImGui::TreePop();
    }
}

