#include "Renderer/Renderer.h"

glm::mat4 Renderer::viewProjMatrix = glm::mat4(1.0f);

void Renderer::Init()
{
    RenderCommand::Init();
}

void Renderer::Shutdown()
{

}

void Renderer::BeginScene(Camera& camera)
{
    RenderCommand::SetClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
    RenderCommand::Clear();

    viewProjMatrix = camera.GetViewProjMatrix();
}

void Renderer::EndScene()
{
    // Rendering should be done last
}

void Renderer::Draw(const SharedPtr<Shader>& shader, const SharedPtr<VertexArray>& vertexArray, const glm::mat4& transform)
{
    shader->Bind();
    shader->SetMat4("model", transform);
    shader->SetMat4("viewProjMatrix", viewProjMatrix);
    RenderCommand::DrawIndexed(vertexArray);
}

void Renderer::OnWindowResize(uint32_t width, uint32_t height)
{

}


