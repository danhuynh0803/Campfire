#ifndef RENDERER_H
#define RENDERER_H

#include "Renderer/RenderCommand.h"
#include "Renderer/RendererAPI.h"
#include "Renderer/Shader.h"
#include "Renderer/Camera.h"
#include "Renderer/Texture.h"

class Renderer
{
public:
    static void Init();
    static void Shutdown();

    static void BeginScene(Camera& camera);
    static void EndScene();


public:
    static void DrawQuad(const glm::mat4& transform, float tilingFactor = 1, const glm::vec4& tintColor = glm::vec4(1.0f));
    static void DrawQuad(const glm::mat4& transform, const SharedPtr<Texture2D>& texture, const glm::vec4& color);

    static void DrawCube(const glm::mat4& transform, const glm::vec4& color);

    static void DrawLine(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2, const glm::vec4& color = glm::vec4(1.0f));

    static void Draw(const SharedPtr<Shader>& shader, const SharedPtr<VertexArray>& vertexArray, const glm::mat4& transform = glm::mat4(1.0f));

    static void OnWindowResize(uint32_t width, uint32_t height);

    static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }


private:
    static glm::mat4 viewProjMatrix;

    //struct SceneData
    //{
    //    glm::mat4 viewProjMatrix;
    //};
};

#endif // RENDERER_H
