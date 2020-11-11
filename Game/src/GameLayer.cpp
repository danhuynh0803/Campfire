#include "../include/GameLayer.h"
#include "Renderer/Renderer.h"

SharedPtr<VertexArray> GameLayer::vao = nullptr;
SharedPtr<Shader> GameLayer::shader = nullptr;

GameLayer::GameLayer()
    : Layer("GameLayer")
{
}

void GameLayer::OnAttach()
{
    vao = VertexArray::Create();
    shader = Shader::Create("tri", "../../Game/src/tri.vert", "../../Game/src/tri.frag");

    float vertices[] = {
         0.0f,  0.5f,  0.0f,    1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f,  0.0f,    0.0f, 1.0f, 0.0f,
         0.5f, -0.5f,  0.0f,    0.0f, 0.0f, 1.0f,
    };

    uint32_t indices[] = {
        0, 1, 2,
    };

    vao->Bind();

    SharedPtr<VertexBuffer> buffer = VertexBuffer::Create(vertices, sizeof(vertices));
    buffer->Bind();

    BufferLayout layout
    {
        { ShaderDataType::FLOAT3, "aPos" },
        { ShaderDataType::FLOAT3, "aColor" },
    };

    buffer->SetLayout(layout);

    buffer->Unbind();

    SharedPtr<IndexBuffer> ibo = IndexBuffer::Create(indices, sizeof(indices)/sizeof(uint32_t));

    vao->SetIndexBuffer(ibo);

    vao->AddVertexBuffer(buffer);

    vao->Unbind();
}

void GameLayer::OnDetach()
{
}

void GameLayer::OnUpdate(float dt)
{
    shader->Bind();
    RenderCommand::DrawIndexed(vao);
    shader->Unbind();

    //Renderer::Draw(shader, vao);
}

void GameLayer::OnImGuiRender()
{
}

void GameLayer::OnEvent(Event& event)
{
}
