#include "Renderer/Renderer.h"
#include "Renderer/Renderer2D.h"

#include <iostream>
#include <array>

// TODO add tiling factor
SharedPtr<Shader> Renderer2D::shader;

struct QuadVertexData
{
    glm::vec3 position;
    glm::vec4 color;
    glm::vec2 uvCoords;
    uint8_t textureID;
};

struct BatchData
{
    static const uint32_t maxQuads = 10000;
    static const uint32_t maxVertices = maxQuads * 4;
    static const uint32_t maxIndices = maxQuads * 6;
    static const uint32_t maxTextureSlots = 32;

    uint32_t indexCount = 0;
    uint32_t quadCount = 0;

    QuadVertexData* quadVertexBufferBase = nullptr;
    QuadVertexData* quadVertexBufferPtr = nullptr;

    SharedPtr<VertexArray> vertexArray;
    SharedPtr<VertexBuffer> vertexBuffer;
    SharedPtr<Texture2D> whiteTexture;

    uint32_t textureSlotIndex = 1; // reserve 0 for default white texture
    std::array<SharedPtr<Texture2D>, maxTextureSlots> textureSlots;

    //std::array<glm::vec4, 4> quadVertexPositions;
};

static BatchData batch;

void Renderer2D::Init()
{
    shader = ShaderManager::Create("quadDefault", "../Campfire/Shaders/quad.vert", "../Campfire/Shaders/quad.frag");
    shader->SetUniformBlock("Matrices", 0);

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

    batch.vertexArray = VertexArray::Create();

    batch.quadVertexBufferBase = new QuadVertexData[batch.maxVertices];
    batch.quadVertexBufferPtr  = batch.quadVertexBufferBase;

    batch.vertexBuffer = VertexBuffer::Create(batch.maxVertices * sizeof(QuadVertexData));

    BufferLayout layout =
    {
        { ShaderDataType::FLOAT3, "aPos" },
        { ShaderDataType::FLOAT4, "aColor" },
        { ShaderDataType::FLOAT2, "aUV" },
        { ShaderDataType::FLOAT , "aTexIndex" }
    };
    batch.vertexBuffer->SetLayout(layout);

    SharedPtr<IndexBuffer> indexBuffer = IndexBuffer::Create(indices.data(), indices.size());
    batch.vertexArray->AddVertexBuffer(batch.vertexBuffer);
    batch.vertexArray->SetIndexBuffer(indexBuffer);

    batch.vertexBuffer->Unbind();
    batch.vertexArray->Unbind();
}

void Renderer2D::Shutdown()
{
}

void Renderer2D::DrawPostProcessQuad(const SharedPtr<Shader>& shader, uint32_t colorAttachmentID)
{
    // TODO need to draw this last and not part of batch
    // or make sure its the last object within the batch
    shader->Bind();
    glBindTextureUnit(0, colorAttachmentID);

    glDisable(GL_DEPTH_TEST);
    //Renderer::Draw(shader, quadVertexArray, glm::mat4(1.0f));
    glEnable(GL_DEPTH_TEST);
}

void Renderer2D::DrawQuad(const glm::mat4& transform, const SharedPtr<Texture2D>& texture, const SharedPtr<Shader>& shader)
{
    shader->Bind();
    texture->Bind();
    //Renderer::Draw(shader, quadVertexArray, transform);
}


void Renderer2D::DrawQuad(const glm::mat4& transform, const SharedPtr<Texture2D>& texture, const glm::vec4& tintColor)
{
    shader->Bind();
    shader->SetFloat4("tintColor", tintColor);

    texture->Bind();

    //Renderer::Draw(shader, quadVertexArray, transform);
}

//=========================================
//--------------Batching-------------------
//=========================================
void Renderer2D::SubmitQuad(const glm::mat4& transform, const SharedPtr<Texture2D>& texture, const glm::vec4& color)
{
    glm::vec2 uv[4] =
    {
        glm::vec2(0.0f, 1.0f),
        glm::vec2(0.0f, 0.0f),
        glm::vec2(1.0f, 0.0f),
        glm::vec2(1.0f, 1.0f)
    };

    // Default quad vertices
    std::array<glm::vec4, 4> pos =
    {
        transform * glm::vec4(-1.0f, 1.0f, 0.0f, 1.0f),
        transform * glm::vec4(-1.0f,-1.0f, 0.0f, 1.0f),
        transform * glm::vec4( 1.0f,-1.0f, 0.0f, 1.0f),
        transform * glm::vec4( 1.0f, 1.0f, 0.0f, 1.0f),
    };

    // Process new data
    for (int i = 0; i < 4; ++i)
    {
        batch.quadVertexBufferPtr->position = glm::vec3(pos[i].x, pos[i].y, pos[i].z);
        batch.quadVertexBufferPtr->color = color;
        batch.quadVertexBufferPtr->uvCoords = uv[i];
        batch.quadVertexBufferPtr->textureID = 0;

        batch.quadVertexBufferPtr++;
    }

    batch.quadCount++;
    batch.indexCount += 6;
}

void Renderer2D::DrawBatch()
{
    if (batch.indexCount == 0) { return; } // No quads submitted to draw

    shader->Bind();
    shader->SetMat4("model", glm::mat4(1.0f));

    // Bind all submitted textures
    for (uint32_t i = 0; i < batch.textureSlotIndex; ++i)
    {
        //batch.textureSlots[i]->Bind();
    }

    batch.vertexArray->Bind();
    glDrawElements(GL_TRIANGLES, batch.indexCount, GL_UNSIGNED_INT, (void*)0);
    batch.vertexArray->Unbind();

    // Clear out quadList for next frame
    //quadList.clear();
    batch.indexCount = 0;
    batch.quadCount = 0;
    batch.quadVertexBufferPtr = batch.quadVertexBufferBase; // reset
}


void Renderer2D::BeginScene(Camera& camera)
{
}

void Renderer2D::EndScene()
{
    // Set new data
    uint32_t dataSize = (uint32_t)((uint8_t*)batch.quadVertexBufferPtr - (uint8_t*)batch.quadVertexBufferBase);
    batch.vertexBuffer->SetData(batch.quadVertexBufferBase, dataSize);

    Renderer2D::DrawBatch();
}


