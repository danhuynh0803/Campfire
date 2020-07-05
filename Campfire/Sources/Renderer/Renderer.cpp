#include "Renderer/Renderer.h"

void Renderer::Init()
{
    RenderCommand::Init();
}

void Renderer::Shutdown()
{

}

void Renderer::BeginScene(Camera& camera)
{

}

void Renderer::EndScene()
{

}

void Renderer::Draw(const SharedPtr<Shader>& shader, const SharedPtr<VertexArray>& vertexArray, const glm::mat4& transform)
{

}

void Renderer::OnWindowResize(uint32_t width, uint32_t height)
{

}


