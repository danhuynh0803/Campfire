#include "OpenGL/OpenGLRendererAPI.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

void OpenGLRendererAPI::Init()
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 16);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
{
    glClearColor(color.r, color.g, color.b, color.a);
}

void OpenGLRendererAPI::Clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLRendererAPI::SetDrawMode(const DrawMode& mode)
{
    switch (mode)
    {
        default:
        case DrawMode::SHADED:
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            break;
        case DrawMode::WIREFRAME:
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
}

void OpenGLRendererAPI::DrawIndexed(const SharedPtr<VertexArray>& vertexArray)
{
    vertexArray->Bind();
    glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, (void*)0);
    vertexArray->Unbind();
}

void OpenGLRendererAPI::DrawIndexedLines(const SharedPtr<VertexArray>& vertexArray)
{
    vertexArray->Bind();
    glDrawElements(GL_LINES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, (void*)0);
    vertexArray->Unbind();
}

void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    glViewport(x, y, width, height);
}

