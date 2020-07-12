#include "Scene/Skybox.h"


void Skybox::OnUpdate(float dt)
{
}

void Skybox::Load(Face face, const std::string& filepath)
{
    faces[static_cast<int>(face)] = filepath;

    textureCube = TextureCube::Create(faces);
}

void Skybox::Load(const std::vector<std::string>& filepaths)
{
    faces = filepaths;
    textureCube = TextureCube::Create(faces);
}

void Skybox::DrawSkybox()
{
    // test draw skybox
    //glDepthMask(GL_FALSE);
    //skyboxShader->Bind();
    //skyboxTexture->Bind();
    //vertexArray->Bind();
    //// Draw cube
    //glDrawArrays(GL_TRIANGLES, 0, 36);
    //vertexArray->Unbind();
    //glDepthMask(GL_TRUE);
}

