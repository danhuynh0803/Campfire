#include "RenderLayer.h"

#include "JobSystem/JobSystem.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Core/Timer.h"

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
    shader = Shader("../Campfire/Shaders/tri.vert", "../Campfire/Shaders/tri.frag");
    GLfloat vertices[] =
    {
        // Position
        0.0f,  0.1f,  0.0f,
       -0.1f, -0.1f,  0.0f,
        0.1f, -0.1f,  0.0f,
    };

    GLuint VBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);   
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void RenderLayer::OnDetach()
{
}

void RenderLayer::DrawTriangles()
{
    shader.use();
    glm::mat4 model = glm::mat4(1.0f);
    glm::vec3 scale = glm::vec3(0.05f);

    model = glm::translate(model, pos);
    model = glm::scale(model, scale);

    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniform4f(glGetUniformLocation(shader.ID, "color"), color.r, color.g, color.b, color.a);

    // Create test triangle
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}

void RenderLayer::OnUpdate()
{  
#define S
#ifdef S
    // Standard way
    Timer timer("Standard triangle draw");
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
#else
    // Jobsystem way
    Timer timer("Jobsystem triangle draw");
    for (int i = 0; i < triangleHeight; ++i)
    {
        for (int j = 0; j < triangleWidth; ++j)
        {
            float u = (float)j / triangleWidth;
            float v = (float)i / triangleHeight;
            float x = u * 2.0f - 1.0f;
            float y = v * 2.0f - 1.0f;
            pos = glm::vec3(x, y, 0.0f);
            color = glm::vec4(u, v, 0.0f, 1.0f);
            DrawTriangles();
        }
    }
#endif
}


void RenderLayer::OnImGuiRender()
{

}

void RenderLayer::OnEvent(Event& event)
{

}
