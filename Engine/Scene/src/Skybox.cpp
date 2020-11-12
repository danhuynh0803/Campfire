#include <glad/glad.h>

#include "Scene/Skybox.h"
#include "Core/FileSystem.h"
#include "imgui.h"

Skybox::Skybox()
{
    Init();
}

void Skybox::Init()
{
    shader = ShaderManager::Get("skybox");

    shader->SetUniformBlock("Camera", 0);

    // TODO should group this with batcher, but might
    // be okay since its just one additional draw call
    // since we'd only render one skybox
    GLfloat vertices[] = {
        // Position
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f
    };

    vertexArray = VertexArray::Create();
    SharedPtr<VertexBuffer> buffer = VertexBuffer::Create(vertices, sizeof(vertices));

    BufferLayout layout =
    {
        { ShaderDataType::FLOAT3, "aPos" }
    };
    buffer->SetLayout(layout);

    vertexArray->AddVertexBuffer(buffer);
}

void Skybox::OnUpdate(float dt)
{
}

void Skybox::OnImGuiRender()
{
    ImGui::Begin("Skybox");

    ImGui::Text("Right");
    if (ImGui::ImageButton((ImTextureID)faceTex2D[0]->GetRenderID(), ImVec2(128, 128), ImVec2(0,1), ImVec2(1,0), -1, ImVec4(0,0,0,0), ImVec4(0.9, 0.9, 0.9, 1.0)))
    {
        std::string path = FileSystem::OpenFile("*.png");
        if (path != "")
        {
            faceTex2D[0] = Texture2D::Create(path);
            facePaths[0] = path;
        }
    }
    ImGui::Separator();

    ImGui::Text("Left");
    if (ImGui::ImageButton((ImTextureID)faceTex2D[1]->GetRenderID(), ImVec2(128, 128), ImVec2(0,1), ImVec2(1,0), -1, ImVec4(0,0,0,0), ImVec4(0.9, 0.9, 0.9, 1.0)))
    {
        std::string path = FileSystem::OpenFile("*.png");
        if (path != "")
        {
            faceTex2D[1] = Texture2D::Create(path);
            facePaths[1] = path;
        }
    }
    ImGui::Separator();

    ImGui::Text("Top");
    if (ImGui::ImageButton((ImTextureID)faceTex2D[2]->GetRenderID(), ImVec2(128, 128), ImVec2(0,1), ImVec2(1,0), -1, ImVec4(0,0,0,0), ImVec4(0.9, 0.9, 0.9, 1.0)))
    {
        std::string path = FileSystem::OpenFile("*.png");
        if (path != "")
        {
            faceTex2D[2] = Texture2D::Create(path);
            facePaths[2] = path;
        }
    }
    ImGui::Separator();

    ImGui::Text("Bottom");
    if (ImGui::ImageButton((ImTextureID)faceTex2D[3]->GetRenderID(), ImVec2(128, 128), ImVec2(0,1), ImVec2(1,0), -1, ImVec4(0,0,0,0), ImVec4(0.9, 0.9, 0.9, 1.0)))
    {
        std::string path = FileSystem::OpenFile("*.png");
        if (path != "")
        {
            faceTex2D[3] = Texture2D::Create(path);
            facePaths[3] = path;
        }
    }
    ImGui::Separator();

    ImGui::Text("Front");
    if (ImGui::ImageButton((ImTextureID)faceTex2D[4]->GetRenderID(), ImVec2(128, 128), ImVec2(0,1), ImVec2(1,0), -1, ImVec4(0,0,0,0), ImVec4(0.9, 0.9, 0.9, 1.0)))
    {
        std::string path = FileSystem::OpenFile("*.png");
        if (path != "")
        {
            faceTex2D[4] = Texture2D::Create(path);
            facePaths[4] = path;
        }
    }
    ImGui::Separator();

    ImGui::Text("Back");
    if (ImGui::ImageButton((ImTextureID)faceTex2D[5]->GetRenderID(), ImVec2(128, 128), ImVec2(0,1), ImVec2(1,0), -1, ImVec4(0,0,0,0), ImVec4(0.9, 0.9, 0.9, 1.0)))
    {
        std::string path = FileSystem::OpenFile("*.png");
        if (path != "")
        {
            faceTex2D[5] = Texture2D::Create(path);
            facePaths[5] = path;
        }
    }
    ImGui::Separator();

    if (ImGui::Button("Rebuild TexCube"))
    {
        textureCube = TextureCube::Create(facePaths);
    }

    ImGui::End();
}

void Skybox::Load(Face face, const std::string& filepath)
{
    facePaths[static_cast<int>(face)] = filepath;

    textureCube = TextureCube::Create(facePaths);
}

void Skybox::Load(const std::vector<std::string>& filepaths)
{
    facePaths = filepaths;
    for (size_t i = 0; i < faceTex2D.size(); ++i)
    {
        faceTex2D[i] = Texture2D::Create(filepaths[i]);
    }
    textureCube = TextureCube::Create(facePaths);
}

void Skybox::DrawSkybox()
{
    // test draw skybox
    glDepthMask(GL_FALSE);
    shader->Bind();
    textureCube->Bind();
    vertexArray->Bind();
    // Draw cube
    glDrawArrays(GL_TRIANGLES, 0, 36);
    vertexArray->Unbind();
    glDepthMask(GL_TRUE);
}

