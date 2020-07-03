#ifndef MODEL_H
#define MODEL_H

#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "GlObject.h"
#include "Texture.h"
#include "ShaderOld.h"
#include "Mesh.h"

class Model : public GlObject
{
public:
    Model (const char* path)
    {
        LoadModel(path);
        InitRenderData();
    }

    void Draw(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale)
    {
        for (Mesh mesh : meshes)
        {
            mesh.Draw(this->shader, pos, rot, scale);
        }
    }

    void InitRenderData() {}

private:
    std::vector<Mesh> meshes;
    std::string directory;
    void LoadModel(std::string path);
    void ProcessNode(aiNode* node, const aiScene* scene);
    Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};

#endif // MODEL_H
