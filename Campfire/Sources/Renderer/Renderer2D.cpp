#include "Renderer/Renderer.h"
#include "Renderer/Renderer2D.h"

#include <iostream>

// TODO add tiling factor
SharedPtr<VertexArray> Renderer2D::quadVertexArray;
SharedPtr<IndexBuffer> Renderer2D::quadIndexBuffer;
SharedPtr<Shader>  Renderer2D::shader;

// TODO set a max amount of batched quads
// need to do so in order to have a large enough buffer
// that can later swap data from without the need to rebuild the buffer

BatchSpecification batch;

void Renderer2D::Init()
{
    shader = ShaderManager::Create("quadDefault", "../Campfire/Shaders/quad.vert", "../Campfire/Shaders/quad.frag");
    shader->SetUniformBlock("Matrices", 0);

    float vertices[] =
    {
       -1.0f,  1.0f,  0.0f,   0.0f, 1.0f,
       -1.0f, -1.0f,  0.0f,   0.0f, 0.0f,
        1.0f, -1.0f,  0.0f,   1.0f, 0.0f,
        1.0f,  1.0f,  0.0f,   1.0f, 1.0f
    };

    std::vector<uint32_t> indices;
    for (uint32_t count = 0; count < batch.maxQuads; ++count)
    {
        indices.insert(indices.end(),
            {
                0 + (4 * count), 1 + (4 * count), 2 + (4 * count),
                2 + (4 * count), 3 + (4 * count), 0 + (4 * count)
            }
        );
    }

    quadVertexArray = VertexArray::Create();

    SharedPtr<VertexBuffer> buffer = VertexBuffer::Create(vertices, sizeof(vertices));
    BufferLayout layout =
    {
        { ShaderDataType::FLOAT3, "aPos" },
        { ShaderDataType::FLOAT4, "aColor" },
        { ShaderDataType::FLOAT2, "aUV" }
    };
    buffer->SetLayout(layout);

    //SharedPtr<IndexBuffer> indexBuffer = IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
    quadIndexBuffer = IndexBuffer::Create(indices.data(), indices.size());

    quadVertexArray->AddVertexBuffer(buffer);
    quadVertexArray->SetIndexBuffer(quadIndexBuffer);

    buffer->Unbind();
    quadVertexArray->Unbind();
}

void Renderer2D::Shutdown()
{
}

void Renderer2D::DrawPostProcessQuad(const SharedPtr<Shader>& shader, uint32_t colorAttachmentID)
{
    shader->Bind();
    glBindTextureUnit(0, colorAttachmentID);

    glDisable(GL_DEPTH_TEST);
    Renderer::Draw(shader, quadVertexArray, glm::mat4(1.0f));
    glEnable(GL_DEPTH_TEST);
}

void Renderer2D::DrawQuad(const glm::mat4& transform, const SharedPtr<Texture2D>& texture, const SharedPtr<Shader>& shader)
{
    shader->Bind();
    texture->Bind();
    Renderer::Draw(shader, quadVertexArray, transform);
}


void Renderer2D::DrawQuad(const glm::mat4& transform, const SharedPtr<Texture2D>& texture, const glm::vec4& tintColor)
{
    shader->Bind();
    shader->SetFloat4("tintColor", tintColor);

    texture->Bind();

    Renderer::Draw(shader, quadVertexArray, transform);
}

std::vector<Renderer2D::QuadData> Renderer2D::quadList;
void Renderer2D::SubmitQuad(const glm::mat4& transform, const SharedPtr<Texture2D>& texture, const glm::vec4& tintColor)
{
    // Default quad vertices
    std::array<glm::vec4, 4> transformedVerts =
    {
        transform * glm::vec4(-1.0f, 1.0f, 0.0f, 1.0f),
        transform * glm::vec4(-1.0f,-1.0f, 0.0f, 1.0f),
        transform * glm::vec4( 1.0f,-1.0f, 0.0f, 1.0f),
        transform * glm::vec4( 1.0f, 1.0f, 0.0f, 1.0f),
    };

    Renderer2D::QuadData quadData =
    {
        transformedVerts,
        tintColor,
        0 // TODO
    };

    batch.quadCount++;
    batch.indexCount += 6;

    quadList.push_back(quadData);
}

void Renderer2D::DrawBatch()
{
    // Create new VAO to house all quad data
    quadVertexArray = VertexArray::Create();

    std::vector<float> vertexData;
    //std::vector<uint32_t> indices;

    uint32_t count = 0;
    for (const auto& quad : quadList)
    {
        for (int i = 0; i < 4; ++i)
        {
            glm::vec4 pos = quad.vertices[i];
            glm::vec4 color = quad.tintColor;
            vertexData.insert(vertexData.end(),
                {
                    pos.x, pos.y, pos.z,
                    color.r, color.g, color.b, color.a,
                    0.0f, 1.0f
                }
            );
        }

//        indices.insert(indices.end(),
//            {
//                0 + (4 * count), 1 + (4 * count), 2 + (4 * count),
//                2 + (4 * count), 3 + (4 * count), 0 + (4 * count)
//            }
//        );
//
//        count++;
    }

    SharedPtr<VertexBuffer> buffer = VertexBuffer::Create(vertexData.data(), vertexData.size() * sizeof(float));
    BufferLayout layout =
    {
        { ShaderDataType::FLOAT3, "aPos" },
        { ShaderDataType::FLOAT4, "aColor" },
        { ShaderDataType::FLOAT2, "aUV" },
    };
    buffer->SetLayout(layout);

    //SharedPtr<IndexBuffer> indexBuffer = IndexBuffer::Create(indices.data(), indices.size());

    quadVertexArray->AddVertexBuffer(buffer);
    quadVertexArray->SetIndexBuffer(quadIndexBuffer);

    shader->Bind();
    shader->SetMat4("model", glm::mat4(1.0f));

    quadVertexArray->Bind();
    glDrawElements(GL_TRIANGLES, batch.indexCount, GL_UNSIGNED_INT, (void*)0);
    quadVertexArray->Unbind();

    //Renderer::Draw(shader, quadVertexArray, glm::mat4(1.0f));

    // Clear out quadList for next frame
    quadList.clear();
    batch.indexCount = 0;
    batch.quadCount = 0;
}


void Renderer2D::BeginScene(Camera& camera)
{
}

void Renderer2D::EndScene()
{
}


