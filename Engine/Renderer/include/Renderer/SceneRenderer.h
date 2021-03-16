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
    SceneRenderer();
    static SharedPtr<SceneRenderer> instance;

    static SharedPtr<SceneRenderer> Get() { return instance; }

    void Init();
    void Shutdown();
    void OnWindowResize(uint32_t width, uint32_t height);

    void BeginSceneWithoutClear(const SharedPtr<Scene>& scene, const Camera& camera);
    void BeginScene(const SharedPtr<Scene>& scene, const Camera& camera);
    void EndScene();

    void SubmitMesh(const SharedPtr<Mesh>& mesh, const glm::mat4& transform = glm::mat4(1.0f), SharedPtr<MaterialInstance> overrideMaterial = nullptr);

private:
    void SubmitCamera(const Camera& camera);
    void SubmitLights(const SharedPtr<Scene>& scene);

private:
    SharedPtr<UniformBuffer> uboCamera;
    SharedPtr<UniformBuffer> uboLights;
    SharedPtr<Shader> shader;
};
