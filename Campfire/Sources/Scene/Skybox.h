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

    Skybox();
    ~Skybox() = default;

    void Init();

    void OnUpdate(float dt);
    void OnImGuiRender();

    std::vector<std::string> GetFacePaths() { return facePaths; }
    void Load(Face face, const std::string& filepath);
    void Load(const std::vector<std::string>& filepaths);

    void DrawSkybox();

private:
    SharedPtr<VertexArray> vertexArray;
    SharedPtr<TextureCube> textureCube;
    SharedPtr<Shader> shader;
    std::vector<std::string> facePaths;

    // Just for ImGui display
    // TODO maybe find some other way
    std::array<SharedPtr<Texture2D>, 6> faceTex2D;
};

#endif // SKYBOX_H
