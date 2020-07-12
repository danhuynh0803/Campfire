#ifndef SKYBOX_H
#define SKYBOX_H

#include <vector>
#include "Core/Base.h"
#include "Renderer/Texture.h"
#include "Renderer/Shader.h"

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

    static void OnUpdate(float dt);

    static void Load(Face face, const std::string& filepath);
    static void Load(const std::vector<std::string>& filepaths);

    static void DrawSkybox();

private:
    static SharedPtr<TextureCube> textureCube;
    static SharedPtr<Shader> shader;

    static std::vector<std::string> faces;
};

#endif // SKYBOX_H
