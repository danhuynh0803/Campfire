#include "Scene/Skybox.h"
#include <glad/glad.h>

void Skybox::Init()
{
    shader = ShaderManager::Create("skybox", "../Campfire/Shaders/skybox.vert", "../Campfire/Shaders/skybox.frag");

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
    glDepthMask(GL_FALSE);
    shader->Bind();
    textureCube->Bind();
    vertexArray->Bind();
    // Draw cube
    glDrawArrays(GL_TRIANGLES, 0, 36);
    vertexArray->Unbind();
    glDepthMask(GL_TRUE);
}

