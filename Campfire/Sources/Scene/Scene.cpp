#include "Scene/Scene.h"
#include "Core/Random.h"
#include <imgui.h>

#include "Renderer/Renderer.h"

Scene::Scene(bool isNewScene)
{
    //==============================================
    // Camera UBO
    uboCamera = UniformBuffer::Create();
    BufferLayout uboLayout =
    {
        { ShaderDataType::MAT4, "view" },
        { ShaderDataType::MAT4, "proj" },
        { ShaderDataType::MAT4, "viewProj" }
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
        { ShaderDataType::FLOAT4, "attenFactors" }
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
    //==============================================

    // Default objects within each new scene
    // if not loading object via scene file
    if (isNewScene) { Init(); }
}


void Scene::Init()
{
    /*
       Scene should be initialized with the following:
       1) Camera
       2) Directional light
    */
    auto& mainCamera = CreateEntity("Camera");
    mainCamera.GetComponent<TransformComponent>().position = glm::vec3(0.0f, 0.0f, 5.0f);
    mainCamera.AddComponent<CameraComponent>();

    auto& directionalLight = CreateEntity("Directional Light");
    directionalLight.GetComponent<TransformComponent>().position = glm::vec3(0.0f, 3.0f, 0.0f);
    directionalLight.AddComponent<LightComponent>();

    // Setup default skybox
    skybox = CreateUniquePtr<Skybox>();
    std::vector<std::string> skyboxTextures =
    {
        "../Assets/Textures/Skyboxes/blue/right.png",
        "../Assets/Textures/Skyboxes/blue/left.png",
        "../Assets/Textures/Skyboxes/blue/top.png",
        "../Assets/Textures/Skyboxes/blue/bottom.png",
        "../Assets/Textures/Skyboxes/blue/front.png",
        "../Assets/Textures/Skyboxes/blue/back.png"
    };
    skybox->Load(skyboxTextures);
}

void Scene::OnUpdate(float dt)
{
    // Update all entities
    // Based on the lua script attached
    // also if theres a rigidbody attached
    {
        auto group = registry.group<ScriptComponent>(entt::get<TransformComponent>);
        for (auto entity : group)
        {
            //auto [transformComponent, scriptComponent] = group.get<TransformComponent, ScriptComponent>(entity);

            //script->OnUpdate(dt);
        }
    }

    // Update rigidbodies
    {
        auto group = registry.group<RigidbodyComponent>(entt::get<TransformComponent>);
        for (auto entity : group)
        {
            auto [transformComponent, rigidbodyComponent] = group.get<TransformComponent, RigidbodyComponent>(entity);
        }
    }

}

void Scene::SubmitCamera(const Camera& camera)
{
    // TODO create a camera controller:
    // update active camera (either editor or game camera)
    // Set UBO data based on that active camera
    uboCamera->Bind();
    uboCamera->SetData((void*)glm::value_ptr(camera.GetViewMatrix()), 0, sizeof(glm::mat4));
    uboCamera->SetData((void*)glm::value_ptr(camera.GetProjMatrix()), sizeof(glm::mat4), sizeof(glm::mat4));
    uboCamera->SetData((void*)glm::value_ptr(camera.GetViewProjMatrix()), 2*sizeof(glm::mat4), sizeof(glm::mat4));
    uboCamera->Unbind();
}

// Render scene from perspective of editor camera
void Scene::OnRenderEditor(float dt, const Camera& editorCamera)
{
    SubmitCamera(editorCamera);

    OnRender(dt);
}

// Render scene from perspective of editor camera
void Scene::OnRenderRuntime(float dt)
{
    // Search for the first object in our scene that contains both camera and transform components
    auto group = registry.group<CameraComponent>(entt::get<TransformComponent>);
    for (auto entity : group)
    {
        auto [transformComponent, cameraComponent] = group.get<TransformComponent, CameraComponent>(entity);

        auto gameCamera = cameraComponent.camera;
        gameCamera->RecalculateViewMatrix(transformComponent.position, transformComponent.rotation);
        gameCamera->SetProjection();
        // TODO change function signature to use camera ptr instead of constantly dereferencing
        SubmitCamera(*gameCamera);
        break;
    }

    OnRender(dt);
}

void Scene::OnRender(float dt)
{
    // Send light info to our UBO
    SubmitLights();

    // Draw skybox
    skybox->DrawSkybox();

    // Render particles first since they're transparent
    {
        auto group = registry.group<ParticleSystemComponent>(entt::get<TransformComponent>);
        for (auto entity : group)
        {
            auto [transformComponent, particleSystemComponent] = group.get<TransformComponent, ParticleSystemComponent>(entity);
            if (particleSystemComponent.ps)
            {
                particleSystemComponent.ps->position = transformComponent.position;
                // TODO have this transparency ordering check in renderer instead of PS
                // or swap to OIT eventually
                //particleSystemComponent.ps->OnUpdate(dt, editorCamera.GetPosition());
                particleSystemComponent.ps->OnUpdate(dt, glm::vec3(0.0f));
                particleSystemComponent.ps->Draw(transformComponent);
            }
        }
    }

    // Render opaque meshes
    {
        // Only render objects that have mesh components
        auto group = registry.group<MeshComponent>(entt::get<TransformComponent>);
        for (auto entity : group)
        {
            auto [transformComponent, meshComponent] = group.get<TransformComponent, MeshComponent>(entity);
            if (meshComponent.mesh)
            {
                //meshComponent.mesh->OnUpdate(dt);

                Renderer::SubmitMesh(meshComponent, transformComponent);
            }
        }
    }
}

void Scene::SubmitLights()
{
    auto group = registry.group<LightComponent>(entt::get<TransformComponent>);
    uint32_t numLights = 0;

    uboLights->Bind();

    for (auto entity : group)
    {
        auto [transformComponent, lightComponent] = group.get<TransformComponent, LightComponent>(entity);

        glm::vec4 position = glm::vec4(transformComponent.position, 0.0f);
        glm::vec4 attenFactors =
            glm::vec4(
                lightComponent.constant,
                lightComponent.linear,
                lightComponent.quadratic,
                0.0f // padding
            );

        // position
        uboLights->SetData((void*)glm::value_ptr(position), (3 * sizeof(glm::vec4) * numLights), sizeof(glm::vec4));
        // color
        uboLights->SetData((void*)glm::value_ptr(lightComponent.color), (3 * sizeof(glm::vec4) * numLights) + sizeof(glm::vec4), sizeof(glm::vec4));
        // attenuation factors
        uboLights->SetData((void*)glm::value_ptr(attenFactors), (3 * sizeof(glm::vec4) * numLights) + 2*sizeof(glm::vec4), sizeof(glm::vec4));

        numLights++;
    }

    // Pass number of lights within the scene
    // 25 is max number of lights
    uboLights->SetData(
        &numLights,
        25 * 3 * sizeof(glm::vec4),
        sizeof(uint32_t)
    );

    uboLights->Unbind();
}

void Scene::OnEvent(Event& e)
{

}

void Scene::OnImGuiRender()
{
}

void Scene::SetSkybox(SharedPtr<TextureCube> skyboxTex)
{
    //skybox->Load()
}

Entity Scene::CreateEntity(const std::string& name)
{
    auto entity = Entity(registry.create(), this);

    // Random ID for access in hashmap
    auto ID = Random::UINT64T();

    // Default components all entities should have
    entity.AddComponent<IDComponent>(ID);
    entity.AddComponent<TagComponent>(name);
    entity.AddComponent<TransformComponent>();

    entityMap[ID] = entity;

    return entity;
}

void Scene::RemoveEntity(Entity entity)
{
    entityMap.erase(entity.GetComponent<IDComponent>());
    registry.destroy(entity.GetHandle());
}


