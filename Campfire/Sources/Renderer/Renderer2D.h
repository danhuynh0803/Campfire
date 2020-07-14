#ifndef RENDERER_2D_H
#define RENDERER_2D_H

#include "Core/Base.h"
#include "Renderer/RendererAPI.h"
#include "Renderer/Shader.h"
#include "Renderer/Camera.h"
#include "Renderer/Texture.h"
#include "Renderer/Buffer.h"

class Renderer2D
{
public:
    static void Init();
    static void Shutdown();

    static void BeginScene(Camera& camera);
    static void EndScene();

    static void OnWindowResize(uint32_t width, uint32_t height);
    static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }

public:
    static void DrawPostProcessQuad(const SharedPtr<Shader>& shader, uint32_t colorAttachmentID);

    static void DrawQuad(const glm::mat4& transform, const SharedPtr<Texture2D>& texture, const SharedPtr<Shader>& shader);

    static void DrawQuad(const glm::mat4& transform, const SharedPtr<Texture2D>& texture, const glm::vec4& tintColor = glm::vec4(1.0f));
    static void DrawQuad(const glm::mat4& transform, float tilingFactor = 1, const glm::vec4& tintColor = glm::vec4(1.0f));

    static void DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec4& color = glm::vec4(1.0f));

    static void Draw(const SharedPtr<Shader>& shader, const SharedPtr<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));

private:
    static SharedPtr<VertexArray> quadVertexArray;
    static SharedPtr<IndexBuffer> quadIndexBuffer;
    static SharedPtr<Shader> shader;
};

#endif // RENDERER_2D_H
