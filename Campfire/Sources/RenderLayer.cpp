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

#include "RenderLayer.h"

Camera camera(1600, 900, 0.1f, 100.0f);

SharedPtr<Shader> postprocessShader;


SharedPtr<Texture2D> tex0;
SharedPtr<Texture2D> tex1;
SharedPtr<Texture2D> tex2;
SharedPtr<Texture2D> tex3;
SharedPtr<Texture2D> tex4;

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

    glm::vec3 scale = glm::vec3(0.5f);
    int x = 50;
    int y = 50;
    Timer timer("Quad draw calls");
    for (int i = 0; i < x; ++i)
    {
        for (int j = 0; j < y; ++j)
        {
            glm::mat4 transform = glm::mat4(1.0f);
            transform = glm::translate(transform, glm::vec3(i-x/2, j-x/2, -5.0f));
            transform = glm::scale(transform, scale);
            glm::vec3 tint = glm::vec3((float)i/x, (float)j/y, 1.0f);

            switch (j%10)
            {
                case 0:
                    Renderer2D::SubmitQuad(transform, tex0, glm::vec4(tint, 1.0f));
                    break;
                case 1:
                    Renderer2D::SubmitQuad(transform, tex1, glm::vec4(tint, 1.0f));
                    break;
                case 2:
                    Renderer2D::SubmitQuad(transform, tex2, glm::vec4(tint, 1.0f));
                    break;
                case 3:
                    Renderer2D::SubmitQuad(transform, tex3, glm::vec4(tint, 1.0f));
                    break;
                case 4:
                    Renderer2D::SubmitQuad(transform, tex4, glm::vec4(tint, 1.0f));
                    break;
                default:
                    Renderer2D::SubmitQuad(transform, glm::vec4(tint, 1.0f));
                    break;
            }
        }
    }

    Renderer::EndScene();
}

void RenderLayer::OnImGuiRender()
{
}

void RenderLayer::OnEvent(Event& event)
{
    camera.OnEvent(event);
}
