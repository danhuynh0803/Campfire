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
    shader = Shader::Create("tri", "tri.vert", "tri.frag");

    float vertices[] = {
         0.0f,  0.5f,  0.0f,
        -0.5f, -0.5f,  0.0f,
         0.5f, -0.5f,  0.0f,
    };

    vao->Bind();

    SharedPtr<VertexBuffer> buffer = VertexBuffer::Create(vertices, sizeof(vertices));
    buffer->Bind();

    BufferLayout layout
    {
        { ShaderDataType::FLOAT3, "aPos" }
        //{ ShaderDataType::FLOAT3, "aColor" }
    };

    buffer->SetLayout(layout);

    buffer->Unbind();

    vao->AddVertexBuffer(buffer);

    vao->Unbind();
}

void GameLayer::OnDetach()
{
}

void GameLayer::OnUpdate(float dt)
{
    //Renderer::Draw(shader, vao);
}

void GameLayer::OnImGuiRender()
{
}

void GameLayer::OnEvent(Event& event)
{
}
