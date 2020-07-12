#include "Renderer/Renderer.h"
#include "Renderer/Renderer2D.h"

// TODO add tiling factor
SharedPtr<VertexArray> Renderer2D::quadVertexArray;
SharedPtr<IndexBuffer> Renderer2D::quadIndexBuffer;
SharedPtr<Shader>  Renderer2D::shader;

void Renderer2D::Init()
{
    shader = Shader::Create("triangle", "../Campfire/Shaders/quad.vert", "../Campfire/Shaders/quad.frag");
    shader->SetUniformBlock("Matrices", 0);

    float vertices[] =
    {
       -1.0f,  1.0f,  0.0f,   0.0f, 1.0f,
       -1.0f, -1.0f,  0.0f,   0.0f, 0.0f,
        1.0f, -1.0f,  0.0f,   1.0f, 0.0f,
        1.0f,  1.0f,  0.0f,   1.0f, 1.0f
    };

    uint32_t indices[] =
    {
        0, 1, 2,
        2, 3, 0
    };

    quadVertexArray = VertexArray::Create();

    SharedPtr<VertexBuffer> buffer = VertexBuffer::Create(vertices, sizeof(vertices));
    BufferLayout layout =
    {
        { ShaderDataType::FLOAT3, "inPos" },
        { ShaderDataType::FLOAT2, "inUV" },
    };
    buffer->SetLayout(layout);

    SharedPtr<IndexBuffer> indexBuffer = IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));

    quadVertexArray->AddVertexBuffer(buffer);
    quadVertexArray->SetIndexBuffer(indexBuffer);

    buffer->Unbind();
    quadVertexArray->Unbind();
}

void Renderer2D::Shutdown()
{
}

void Renderer2D::DrawQuad(const glm::mat4& transform, const SharedPtr<Texture2D>& texture, const glm::vec4& tintColor)
{
    shader->Bind();
    shader->SetFloat4("tintColor", tintColor);

    texture->Bind();

    Renderer::Draw(shader, quadVertexArray, transform);
}

//void Renderer2D::DrawQuad(const glm::mat4& transform, float tilingFactor, const glm::vec4& tintColor)
//{
//    shader->Bind();
//    shader->SetFloat("tilingFactor", tilingFactor);
//    shader->SetFloat4("tintColor", tintColor);
//
//    Renderer::Draw(shader, quadVertexArray, transform);
//}

void Renderer2D::BeginScene(Camera& camera)
{
}

void Renderer2D::EndScene()
{
}


