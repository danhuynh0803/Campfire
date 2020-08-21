#include "Renderer/SceneRenderer.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Renderer2D.h"
#include "Scene/Component.h"

#include <glm/gtc/type_ptr.hpp>

SharedPtr<UniformBuffer> SceneRenderer::uboCamera;
SharedPtr<UniformBuffer> SceneRenderer::uboLights;
SharedPtr<Shader> SceneRenderer::shader;

void SceneRenderer::Init()
{
    //==============================================
    // Camera UBO
    uboCamera = UniformBuffer::Create();
    BufferLayout uboLayout =
    {
        { ShaderDataType::MAT4, "view" },
        { ShaderDataType::MAT4, "proj" },
        { ShaderDataType::MAT4, "viewProj" },
        { ShaderDataType::FLOAT4, "pos" }
    };
    uboCamera->SetLayout(uboLayout, 0);
    //==============================================

    //==============================================
    // UBO for lights
    uboLights = UniformBuffer::Create();
    uboLayout =
    {
        { ShaderDataType::FLOAT4, "pos" },
        { ShaderDataType::FLOAT4, "color" },
        { ShaderDataType::FLOAT4, "attenFactors" },
        { ShaderDataType::FLOAT4, "lightDir" }
    };
    uboLights->SetLayout(uboLayout, 1, 26);
    /*
       25 is currently the max number of lights specified within the shader,
       but we pass 26 since it's a bit messy otherwise to set in the bufferlayout.
       Data is stored as the following in the shader:

       Lights[25];
       uint numLights; //which comes after the array

       --------------------------

       In terms of memory layout, it looks like:
       Lights[0]
       Lights[1]
       ...
       ...
       Lights[24]
       uint numLights
     */

    // TODO should be part of material
    shader = ShaderManager::Create("pbr", "../Campfire/Shaders/pbr.vert", "../Campfire/Shaders/pbr.frag");
    shader->Bind();
    shader->SetUniformBlock("Camera", 0);
    shader->SetUniformBlock("LightBuffer", 1);
    shader->Unbind();
}

void SceneRenderer::Shutdown()
{
}

void SceneRenderer::BeginScene(const SharedPtr<Scene>& scene, const Camera& camera)
{
    SubmitCamera(camera);
    SubmitLights(scene);

    RenderCommand::SetClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));
    RenderCommand::Clear();
    Renderer2D::BeginScene(camera);

    // Draw skybox first
    scene->skybox->DrawSkybox();
}

void SceneRenderer::EndScene()
{
    // Where we actually render all scene sprites via batching
    Renderer2D::EndScene();
}

void SceneRenderer::SubmitMesh(const SharedPtr<Mesh>& mesh, const glm::mat4& transform, SharedPtr<Material> overrideMaterial)
{
    shader->Bind();
    shader->SetMat4("model", transform);

    if (overrideMaterial)
    {
        shader->SetInt("albedoMap", 0);
        shader->SetInt("specularMap", 1);
        shader->SetInt("normalMap", 2);
        shader->SetInt("roughnessMap", 3);
        shader->SetInt("ambientOcclusionMap", 4);
        shader->SetInt("skybox", 5);

        if (overrideMaterial->albedoMap)
        {
            overrideMaterial->albedoMap->Bind(0);
        }

        if (overrideMaterial->specularMap)
        {
            overrideMaterial->specularMap->Bind(1);
        }

        if (overrideMaterial->normalMap)
        {
            overrideMaterial->normalMap->Bind(2);
        }

        if (overrideMaterial->roughnessMap)
        {
            overrideMaterial->roughnessMap->Bind(3);
        }

        if (overrideMaterial->ambientOcclusionMap)
        {
            overrideMaterial->ambientOcclusionMap->Bind(4);
        }
    }

    //texCube->Bind(5);

    // TODO Replace with Materials
    for (auto submesh : mesh->GetSubmeshes())
    {
        // Load default white texture of model had no textures
        if (submesh.textures.size() == 0)
        {
            //LOG_WARN("{0}::Warning: No material found, defaulting to white texture", mesh->GetName());
            //shader->SetFloat("texDiffuse", 0);
            //whiteTexture->Bind();
        }
        else
        {
//            for(size_t i = 0; i < submesh.textures.size(); ++i)
//            {
//                auto texture = submesh.textures[i];
//                if (texture)
//                {
//                    shader->SetFloat("texDiffuse", i);
//                    submesh.textures[i]->Bind(i);
//                }
//            }
        }

        RenderCommand::DrawIndexed(submesh.vertexArray);
    }

}

void SceneRenderer::OnWindowResize(uint32_t width, uint32_t height)
{
    RenderCommand::SetViewport(0, 0, width, height);
}

void SceneRenderer::SubmitCamera(const Camera& camera)
{
    uboCamera->Bind();
    uboCamera->SetData((void*)glm::value_ptr(camera.GetViewMatrix()), 0, sizeof(glm::mat4));
    uboCamera->SetData((void*)glm::value_ptr(camera.GetProjMatrix()), sizeof(glm::mat4), sizeof(glm::mat4));
    uboCamera->SetData((void*)glm::value_ptr(camera.GetViewProjMatrix()), 2*sizeof(glm::mat4), sizeof(glm::mat4));
    uboCamera->SetData((void*)glm::value_ptr(camera.pos), 3*sizeof(glm::mat4), sizeof(glm::vec4));
    uboCamera->Unbind();
}

void SceneRenderer::SubmitLights(const SharedPtr<Scene>& scene)
{
    uboLights->Bind();

    auto group = scene->registry.group<LightComponent>(entt::get<TransformComponent>);
    uint32_t numLights = 0;
    uint32_t size = ( 4 * sizeof(glm::vec4) );
    for (auto entity : group)
    {
        auto [transformComponent, lightComponent] = group.get<TransformComponent, LightComponent>(entity);

        glm::vec4 position = glm::vec4(transformComponent.position, 0.0f);

        glm::vec4 attenFactors =
            glm::vec4(
                lightComponent.constant,
                lightComponent.linear,
                lightComponent.quadratic,
                static_cast<uint32_t>(lightComponent.type)
            );

        // position
        uint32_t offset = 0;
        uboLights->SetData((void*)glm::value_ptr(position), size*numLights + offset, sizeof(glm::vec4));

        // color
        offset += sizeof(glm::vec4);
        uboLights->SetData((void*)glm::value_ptr(lightComponent.color), size*numLights + offset, sizeof(glm::vec4));

        // attenuation factors
        offset += sizeof(glm::vec4);
        uboLights->SetData((void*)glm::value_ptr(attenFactors), size*numLights + offset, sizeof(glm::vec4));

        // lightDir
        glm::mat4 transform = transformComponent.GetTransform();
        offset += sizeof(glm::vec4);
        // Based off of +Z direction
        glm::vec4 zDir = transform * glm::vec4(0, 0, 1, 0);
        uboLights->SetData((void*)glm::value_ptr(zDir), size*numLights + offset, sizeof(glm::vec4));

        numLights++;
    }

    // Pass number of lights within the scene
    // 25 is max number of lights
    uboLights->SetData(
        &numLights,
        25 * size,
        sizeof(uint32_t)
    );

    uboLights->Unbind();
}


