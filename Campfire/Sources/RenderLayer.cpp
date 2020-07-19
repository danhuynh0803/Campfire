#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Core/Base.h"
#include "Core/Timer.h"
#include "Core/Input.h"
#include "Core/Time.h"

#include "Renderer/Renderer.h"
#include "Renderer/Shader.h"
#include "Renderer/Renderer2D.h"
#include "Renderer/Camera.h"
#include "Scene/Scene.h"

#include "Particles/ParticleSystem.h"

#include "RenderLayer.h"

Camera camera(1600, 900, 0.1f, 100.0f);

SharedPtr<Shader> postprocessShader;


SharedPtr<Texture2D> tex0;
SharedPtr<Texture2D> tex1;
SharedPtr<Texture2D> tex2;
SharedPtr<Texture2D> tex3;
SharedPtr<Texture2D> tex4;

ParticleSystem ps;

RenderLayer::RenderLayer()
    : Layer("RenderLayer")
{
}

void RenderLayer::OnAttach()
{
    tex0 = Texture2D::Create("../Assets/Textures/awesomeface.png");
    tex1 = Texture2D::Create("../Assets/Textures/uv.png");
    tex2 = Texture2D::Create("../Assets/Textures/wall.jpg");
    tex3 = Texture2D::Create("../Assets/Textures/container.jpg");
    tex4 = Texture2D::Create("../Assets/Textures/flowmap.png");

    ubo = UniformBuffer::Create();
    BufferLayout uboLayout =
    {
        { ShaderDataType::MAT4, "view"},
        { ShaderDataType::MAT4, "proj"},
        { ShaderDataType::MAT4, "viewProj"}
    };
    ubo->SetLayout(uboLayout, 0);

    colorFB = FrameBuffer::Create(1600, 900, 16);

    postprocessShader = ShaderManager::Create("postprocess", "../Campfire/Shaders/postprocess.vert", "../Campfire/Shaders/postprocess.frag");

    ps.position = glm::vec3(0.0f, 0.0f, -2.0f);
    ps.GenerateParticles(100);
}

void RenderLayer::OnDetach()
{
}

void RenderLayer::OnUpdate(float dt)
{
    camera.OnUpdate(dt);

    ps.OnUpdate(dt);

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

    colorFB->Bind();

    Timer timer("Particle draw");
    ps.Draw();

    colorFB->Bind();

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, colorFB->GetRenderID());
    glDrawBuffer(GL_BACK);
    glBlitFramebuffer(0, 0, 1600, 900, 0, 0, 1600, 900, GL_COLOR_BUFFER_BIT, GL_LINEAR);

    Renderer::EndScene();
}

void RenderLayer::OnImGuiRender()
{
}

void RenderLayer::OnEvent(Event& event)
{
    camera.OnEvent(event);
}
