#include "RenderLayer.h"
#include "Renderer/Renderer.h"
#include "JobSystem/JobSystem.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Core/Timer.h"

#include "Renderer/Buffer.h"
#include "Renderer/Shader.h"

unsigned int triangleWidth = 100;
unsigned int triangleHeight = 100;
float red, green;

JobSystem jobSystem;
glm::vec3 pos;
glm::vec4 color;

RenderLayer::RenderLayer()
    : Layer("RenderLayer")
{
}

void RenderLayer::OnAttach()
{
    Renderer::SetAPI(RendererAPI::OpenGL);

    shader = Shader::Create("triangle", "../Campfire/Shaders/tri.vert", "../Campfire/Shaders/tri.frag");
    GLfloat vertices[] =
    {
        // Position
        /*
                |
            0-------3
            |   |   |
          -------------
            |   |   |
            1-------2
                |
        */

       //-0.1f,  0.1f,  0.0f,
       //-0.1f, -0.1f,  0.0f,
       // 0.1f, -0.1f,  0.0f,
       // 0.1f,  0.1f,  0.0f,
       -1.0f,  1.0f,  0.0f,
       -1.0f, -1.0f,  0.0f,
        1.0f, -1.0f,  0.0f,
        1.0f,  1.0f,  0.0f,
    };

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    std::unique_ptr<VertexBuffer> buffer;
    buffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));
    buffer->Bind();
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    GLuint indices[] =
    {
        0, 1, 2,
        2, 3, 0
    };

    indexBuffer.reset(IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(0);

    buffer->Unbind();
    glBindVertexArray(0);
}

void RenderLayer::OnDetach()
{
}

void RenderLayer::DrawTriangles()
{
    shader->Bind();
    glm::mat4 model = glm::mat4(1.0f);
    //glm::vec3 scale = glm::vec3(0.1f);

    model = glm::translate(model, pos);
    //model = glm::scale(model, scale);

    shader->SetFloat("time", static_cast<float>(glfwGetTime()));
    shader->SetMat4("model", model);
    shader->SetFloat4("color", color);    

    // Create test triangle
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexBuffer->GetCount(), GL_UNSIGNED_INT, (void*)0);
    glBindVertexArray(0);
}

void RenderLayer::OnUpdate()
{
    // Standard way
    Timer timer("Standard triangle draw");
    pos = glm::vec3(0.0);
    color = glm::vec4(1.0f);
    DrawTriangles();
    return;

    for (int i = 0; i < triangleHeight; ++i)
    {
        for (int j = 0; j < triangleWidth; ++j)
        {
            float u = (float)j / triangleWidth;
            float v = (float)i / triangleHeight;
            float x = u * 2.0f - 1.0f;
            float y = v * 2.0f - 1.0f;
            pos = glm::vec3(x, y, 0.0f);
            color = glm::vec4(u, v, 1.0f, 1.0f);
            DrawTriangles();
        }
    }
}


void RenderLayer::OnImGuiRender()
{

}

void RenderLayer::OnEvent(Event& event)
{

}
