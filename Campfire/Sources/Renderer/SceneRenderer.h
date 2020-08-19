#pragma once

#include "Core/Base.h"

#include "Renderer/Shader.h"
#include "Renderer/Texture.h"
#include "Renderer/Mesh.h"
#include "Renderer/Material.h"

#include "Scene/Camera.h"

class Scene;

class SceneRenderer
{
public:
    static void Init();
    static void Shutdown();
    static void OnWindowResize(uint32_t width, uint32_t height);

    static void BeginScene(const SharedPtr<Scene>& scene, const Camera& camera);
    static void EndScene();

    static void SubmitMesh(const SharedPtr<Mesh>& mesh, const glm::mat4& transform = glm::mat4(1.0f), SharedPtr<Material> overrideMaterial = nullptr);

private:
    static void SubmitCamera(const Camera& camera);
    static void SubmitLights(const SharedPtr<Scene>& scene);

private:
    static SharedPtr<UniformBuffer> uboCamera;
    static SharedPtr<UniformBuffer> uboLights;
    static SharedPtr<Shader> shader;
};
