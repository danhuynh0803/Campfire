#include "RenderLayer.h"

RenderLayer::RenderLayer()
    : Layer("RenderLayer")
{
}

void RenderLayer::OnAttach()
{
    GLfloat vertices[] =
    {
        0.0f,  0.5f,  0.0f,
       -0.5f, -0.5f,  0.0f,
        0.5f, -0.5f,  0.0f,
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

void RenderLayer::OnUpdate()
{
    // Create test triangle
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}


void RenderLayer::OnImGuiRender()
{

}

void RenderLayer::OnEvent(Event& event)
{

}
