#ifndef SKYBOX_H
#define SKYBOX_H

#include <vector>
#include "Core/Base.h"
#include "Renderer/Texture.h"
#include "Renderer/Shader.h"
#include "Renderer/VertexArray.h"

class Skybox
{
public:

    enum class Face
    {
        RIGHT = 0,
        LEFT,
        TOP,
        BOTTOM,
        FRONT,
        BACK
    };

    Skybox() = default;
    ~Skybox() = default;

    void Init();

    void OnUpdate(float dt);

    void Load(Face face, const std::string& filepath);
    void Load(const std::vector<std::string>& filepaths);

    void DrawSkybox();

private:
    SharedPtr<TextureCube> textureCube;
    SharedPtr<Shader> shader;
    SharedPtr<VertexArray> vertexArray;

    std::vector<std::string> faces;
};

#endif // SKYBOX_H
