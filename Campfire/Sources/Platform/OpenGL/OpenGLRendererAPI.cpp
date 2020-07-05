#include "Platform/OpenGL/OpenGLRendererAPI.h"

#include <glad/glad.h>

void OpenGLRendererAPI::Init()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
}

void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
{
    glClearColor(color.r, color.g, color.b, color.a);
}

void OpenGLRendererAPI::Clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenGLRendererAPI::DrawIndexed(const SharedPtr<VertexArray>& vertexArray)
{
    vertexArray->Bind();
    //glDrawElements(GL_TRIANGLES, vertexArray->GetIndexBuffer->GetCount(), GL_UNSIGNED_INT, (void*)0);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    vertexArray->Unbind();
}


