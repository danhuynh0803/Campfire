#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Core/Base.h"
#include "Core/Timer.h"
#include "Core/Input.h"
#include "Core/Time.h"

#include "Renderer/Renderer.h"
#include "Renderer/Shader.h"
#include "Renderer/Renderer2D.h"
#include "Renderer/Camera.h"
#include "Renderer/Mesh.h"
#include "Scene/Scene.h"
#include "Scene/Skybox.h"

#include "ImGui/ImGuiLayer.h"
#include "Core/FileSystem.h"

#include "Particles/ParticleSystem.h"

#include "RenderLayer.h"

#include <stdint.h> // intptr_t

Camera camera(1600, 900, 0.1f, 100.0f);

SharedPtr<Shader> postprocessShader;

Skybox skybox;

RenderLayer::RenderLayer()
    : Layer("RenderLayer")
{
}

void RenderLayer::OnAttach()
{
    ubo = UniformBuffer::Create();
    BufferLayout uboLayout =
    {
        { ShaderDataType::MAT4, "view"},
        { ShaderDataType::MAT4, "proj"},
        { ShaderDataType::MAT4, "viewProj"}
    };
    ubo->SetLayout(uboLayout, 0);

    colorFB = Framebuffer::Create(1600, 900, 16);

    postprocessShader = ShaderManager::Create("postprocess", "../Campfire/Shaders/postprocess.vert", "../Campfire/Shaders/postprocess.frag");

    skybox.Init();

    std::vector<std::string> skyboxTextures =
    {
        "../Assets/Textures/Skyboxes/blue/right.png",
        "../Assets/Textures/Skyboxes/blue/left.png",
        "../Assets/Textures/Skyboxes/blue/top.png",
        "../Assets/Textures/Skyboxes/blue/bottom.png",
        "../Assets/Textures/Skyboxes/blue/front.png",
        "../Assets/Textures/Skyboxes/blue/back.png"
    };
    skybox.Load(skyboxTextures);
}

void RenderLayer::OnDetach()
{
}

void RenderLayer::OnUpdate(float dt)
{
    camera.OnUpdate(dt);

    if (Input::GetMod(MOD_SHIFT) && Input::GetKeyDown(KEY_R))
    {
        ShaderManager::ReloadShaders();
    }

    // Set UBO data
    ubo->Bind();
    ubo->SetData((void*)glm::value_ptr(camera.GetViewMatrix()), 0, sizeof(glm::mat4));
    ubo->SetData((void*)glm::value_ptr(camera.GetProjMatrix()), sizeof(glm::mat4), sizeof(glm::mat4));
    ubo->SetData((void*)glm::value_ptr(camera.GetViewProjMatrix()), 2*sizeof(glm::mat4), sizeof(glm::mat4));
    ubo->Unbind();

    Renderer::BeginScene(camera);

    skybox.DrawSkybox();

    Renderer::EndScene();
}

void RenderLayer::OnImGuiRender()
{
    // TODO issues with loading, looks like an issue with mixing relative path and absolute path
    //skybox.OnImGuiRender();
}

void RenderLayer::OnEvent(Event& event)
{
    camera.OnEvent(event);
}
