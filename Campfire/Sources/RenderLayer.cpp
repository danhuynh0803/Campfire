#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Core/Base.h"
#include "Core/Timer.h"
#include "RenderLayer.h"
#include "Renderer/Renderer.h"
#include "Renderer/Shader.h"

#include "Core/Input.h"
#include "Core/Time.h"

#include "Renderer/Camera.h"
Camera camera(1600, 900, 0.1f, 100.0f);

unsigned int triangleWidth = 100;
unsigned int triangleHeight = 100;
float red, green;

glm::vec3 pos;
glm::vec4 color;

RenderLayer::RenderLayer()
    : Layer("RenderLayer")
{
}

void RenderLayer::OnAttach()
{
    pos = glm::vec3(0.0);
    color = glm::vec4(1.0f);

    shader = Shader::Create("triangle", "../Campfire/Shaders/tri.vert", "../Campfire/Shaders/tri.frag");
    skyboxShader = Shader::Create("skybox", "../Campfire/Shaders/skybox.vert", "../Campfire/Shaders/skybox.frag");
//    GLfloat vertices[] =
//    {
//       -1.0f,  1.0f,  0.0f,     0.0f, 1.0f,
//       -1.0f, -1.0f,  0.0f,     0.0f, 0.0f,
//        1.0f, -1.0f,  0.0f,     1.0f, 0.0f,
//        1.0f,  1.0f,  0.0f,     1.0f, 1.0f
//    };

        GLfloat vertices[] = {
            // Position           // UV         // Normals
            -1.0f, -1.0f, -1.0f,  0.0f, 0.0f,   0, 0, -1,
             1.0f, -1.0f, -1.0f,  1.0f, 0.0f,   0, 0, -1,
             1.0f,  1.0f, -1.0f,  1.0f, 1.0f,   0, 0, -1,
             1.0f,  1.0f, -1.0f,  1.0f, 1.0f,   0, 0, -1,
            -1.0f,  1.0f, -1.0f,  0.0f, 1.0f,   0, 0, -1,
            -1.0f, -1.0f, -1.0f,  0.0f, 0.0f,   0, 0, -1,

            -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,   0, 0, 1,
             1.0f, -1.0f,  1.0f,  1.0f, 0.0f,   0, 0, 1,
             1.0f,  1.0f,  1.0f,  1.0f, 1.0f,   0, 0, 1,
             1.0f,  1.0f,  1.0f,  1.0f, 1.0f,   0, 0, 1,
            -1.0f,  1.0f,  1.0f,  0.0f, 1.0f,   0, 0, 1,
            -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,   0, 0, 1,

            -1.0f,  1.0f,  1.0f,  1.0f, 0.0f,  -1, 0, 0,
            -1.0f,  1.0f, -1.0f,  1.0f, 1.0f,  -1, 0, 0,
            -1.0f, -1.0f, -1.0f,  0.0f, 1.0f,  -1, 0, 0,
            -1.0f, -1.0f, -1.0f,  0.0f, 1.0f,  -1, 0, 0,
            -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,  -1, 0, 0,
            -1.0f,  1.0f,  1.0f,  1.0f, 0.0f,  -1, 0, 0,

             1.0f,  1.0f,  1.0f,  1.0f, 0.0f,   1, 0, 0,
             1.0f,  1.0f, -1.0f,  1.0f, 1.0f,   1, 0, 0,
             1.0f, -1.0f, -1.0f,  0.0f, 1.0f,   1, 0, 0,
             1.0f, -1.0f, -1.0f,  0.0f, 1.0f,   1, 0, 0,
             1.0f, -1.0f,  1.0f,  0.0f, 0.0f,   1, 0, 0,
             1.0f,  1.0f,  1.0f,  1.0f, 0.0f,   1, 0, 0,

            -1.0f, -1.0f, -1.0f,  0.0f, 1.0f,   0, -1, 0,
             1.0f, -1.0f, -1.0f,  1.0f, 1.0f,   0, -1, 0,
             1.0f, -1.0f,  1.0f,  1.0f, 0.0f,   0, -1, 0,
             1.0f, -1.0f,  1.0f,  1.0f, 0.0f,   0, -1, 0,
            -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,   0, -1, 0,
            -1.0f, -1.0f, -1.0f,  0.0f, 1.0f,   0, -1, 0,

            -1.0f,  1.0f, -1.0f,  0.0f, 1.0f,   0, 1, 0,
             1.0f,  1.0f, -1.0f,  1.0f, 1.0f,   0, 1, 0,
             1.0f,  1.0f,  1.0f,  1.0f, 0.0f,   0, 1, 0,
             1.0f,  1.0f,  1.0f,  1.0f, 0.0f,   0, 1, 0,
            -1.0f,  1.0f,  1.0f,  0.0f, 0.0f,   0, 1, 0,
            -1.0f,  1.0f, -1.0f,  0.0f, 1.0f,   0, 1, 0
        };

    vertexArray = VertexArray::Create();
    vertexArray->Bind();

    ubo = UniformBuffer::Create();
    BufferLayout uboLayout =
    {
        { ShaderDataType::MAT4, "view"},
        { ShaderDataType::MAT4, "proj"},
        { ShaderDataType::MAT4, "viewProj"}
    };
    ubo->SetLayout(uboLayout, 0);
    shader->SetUniformBlock("Matrices", 0);
    skyboxShader->SetUniformBlock("Matrices", 0);


    SharedPtr<VertexBuffer> buffer = VertexBuffer::Create(vertices, sizeof(vertices));
    BufferLayout layout =
    {
        { ShaderDataType::FLOAT3, "inPos"},
        { ShaderDataType::FLOAT2, "inUV"},
        { ShaderDataType::FLOAT3, "inNormal"}
    };
    buffer->SetLayout(layout);

    GLuint indices[] =
    {
        0, 1, 2,
        2, 3, 0
    };

    indexBuffer = IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));

    vertexArray->AddVertexBuffer(buffer);
    vertexArray->SetIndexBuffer(indexBuffer);

    buffer->Unbind();
    vertexArray->Unbind();

    texture = Texture2D::Create("../Assets/Textures/uv.png");
    std::vector<std::string> faces =
    {
        "../Assets/Textures/skybox/right.jpg",
        "../Assets/Textures/skybox/left.jpg",
        "../Assets/Textures/skybox/top.jpg",
        "../Assets/Textures/skybox/bottom.jpg",
        "../Assets/Textures/skybox/front.jpg",
        "../Assets/Textures/skybox/back.jpg"
    };
    skyboxTexture = TextureCube::Create(faces);
}

void RenderLayer::OnDetach()
{
}

void RenderLayer::DrawTriangles()
{
    //glm::vec3 scale = glm::vec3(0.1f);
    //model = glm::scale(model, scale);
    ubo->Bind();
    uint32_t index = 0;
    glBufferSubData(GL_UNIFORM_BUFFER, index * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(camera.GetViewMatrix()));
    index++;
    glBufferSubData(GL_UNIFORM_BUFFER, index * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(camera.GetProjMatrix()));
    index++;
    glBufferSubData(GL_UNIFORM_BUFFER, index * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(camera.GetViewProjMatrix()));
    ubo->Unbind();


    // test draw skybox
    glDepthMask(GL_FALSE);
    skyboxShader->Bind();
    skyboxTexture->Bind();
    vertexArray->Bind();
    glDrawArrays(GL_TRIANGLES, 0, 36);
    vertexArray->Unbind();
    glDepthMask(GL_TRUE);

    shader->Bind();
    shader->SetFloat("time", static_cast<float>(glfwGetTime()));

    texture->Bind();
    Renderer::Draw(shader, vertexArray);

}

void RenderLayer::OnUpdate(float timestep)
{
    camera.OnUpdate(timestep);
    // Testing input controller
    if (Input::GetKeyDown(KEY_UP))
    {
        pos.y += 0.1f;
    }
    if (Input::GetKeyUp(KEY_DOWN))
    {
        pos.y -= 0.1f;
    }

    DrawTriangles();
}

void RenderLayer::OnImGuiRender()
{

}

void RenderLayer::OnEvent(Event& event)
{
    camera.OnEvent(event);
}
