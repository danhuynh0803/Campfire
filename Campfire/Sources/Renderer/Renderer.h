#ifndef RENDERER_H
#define RENDERER_H

#include "Renderer/RenderCommand.h"
#include "Renderer/RendererAPI.h"
#include "Renderer/Shader.h"
#include "Renderer/Camera.h"

class Renderer
{
public:
    static void Init();
    static void Shutdown();

    static void BeginScene(Camera& camera);
    static void EndScene();

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
