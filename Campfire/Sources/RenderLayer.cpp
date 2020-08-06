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

RenderLayer::RenderLayer()
    : Layer("RenderLayer")
{
}

void RenderLayer::OnAttach()
{
    colorFB = Framebuffer::Create(1600, 900, 16);

    postprocessShader = ShaderManager::Create("postprocess", "../Campfire/Shaders/postprocess.vert", "../Campfire/Shaders/postprocess.frag");

}

void RenderLayer::OnDetach()
{
}

void RenderLayer::OnUpdate(float dt)
{
    if (Input::GetMod(MOD_SHIFT) && Input::GetKeyDown(KEY_R))
    {
        ShaderManager::ReloadShaders();
    }
}

void RenderLayer::OnImGuiRender()
{
    // TODO issues with loading, looks like an issue with mixing relative path and absolute path
    //skybox.OnImGuiRender();
}

void RenderLayer::OnEvent(Event& event)
{
    //camera.OnEvent(event);
}
