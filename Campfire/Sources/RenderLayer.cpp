#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Core/Base.h"
#include "Core/Timer.h"
#include "Core/Input.h"
#include "Core/Time.h"

#include "Renderer/Renderer.h"
#include "Renderer/Shader.h"
#include "Renderer/Camera.h"
#include "Renderer/Renderer2D.h"

#include "Scene/Scene.h"

#include "RenderLayer.h"

Camera camera(1600, 900, 0.1f, 100.0f);

RenderLayer::RenderLayer()
    : Layer("RenderLayer")
{
}

void RenderLayer::OnAttach()
{
    texture2D = Texture2D::Create("../Assets/Textures/awesomeface.png");

    ubo = UniformBuffer::Create();
    BufferLayout uboLayout =
    {
        { ShaderDataType::MAT4, "view"},
        { ShaderDataType::MAT4, "proj"},
        { ShaderDataType::MAT4, "viewProj"}
    };
    ubo->SetLayout(uboLayout, 0);
}

void RenderLayer::OnDetach()
{
}

void RenderLayer::OnUpdate(float dt)
{
    camera.OnUpdate(dt);

    // Set UBO data
    ubo->Bind();
    uint32_t index = 0;
    glBufferSubData(GL_UNIFORM_BUFFER, index * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(camera.GetViewMatrix()));
    index++;
    glBufferSubData(GL_UNIFORM_BUFFER, index * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(camera.GetProjMatrix()));
    index++;
    glBufferSubData(GL_UNIFORM_BUFFER, index * sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(camera.GetViewProjMatrix()));
    ubo->Unbind();



    glm::vec3 scale = glm::vec3(0.5f);
    int x = 10;
    int y = 10;
    Timer timer("Quad draw calls");
    for (int i = 0; i < x; ++i)
    {
        for (int j = 0; j < y; ++j)
        {
            glm::mat4 transform = glm::mat4(1.0f);
            transform = glm::translate(transform, glm::vec3(i, j, 0.0f));
            transform = glm::scale(transform, scale);
            glm::vec3 tint = glm::vec3((float)i/x, (float)j/y, 1.0f);
            Renderer2D::DrawQuad(transform, texture2D, glm::vec4(tint, 1.0f));
        }
    }
}

void RenderLayer::OnImGuiRender()
{
}

void RenderLayer::OnEvent(Event& event)
{
    camera.OnEvent(event);
}
