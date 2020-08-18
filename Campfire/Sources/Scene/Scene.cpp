#include "Scene/Scene.h"
#include "Core/Random.h"
#include <imgui.h>

#include "Renderer/Renderer.h"
#include "Renderer/Renderer2D.h"
#include "Physics/PhysicsManager.h"
#include "Scripting/CameraController.h"

Scene::Scene(bool isNewScene)
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
       3) Test cube
    */
    auto mainCamera = CreateEntity("Camera");
    mainCamera.GetComponent<TransformComponent>().position = glm::vec3(0.0f, 0.0f, 10.0f);
    mainCamera.AddComponent<CameraComponent>();
    mainCamera.AddComponent<NativeScriptComponent>().Bind<Script::CameraController>();

    auto directionalLight = CreateEntity("Directional Light");
    directionalLight.GetComponent<TransformComponent>().position = glm::vec3(0.0f, 5.0f, 2.0f);
    directionalLight.GetComponent<TransformComponent>().rotation = glm::vec3(120.0f, 0.0f, 0.0f);
    directionalLight.AddComponent<LightComponent>();


    int totalRow = 2;
    int totalCol = 2;
    float xDisp = 3.0f;
    float yDisp = 3.0f;
//    for (int y = 0; y <= totalCol; ++y)
//    {
//        for (int x = 0; x <= totalRow; ++x)
//        {
//            auto pointLight = CreateEntity("Point Light");
//            glm::vec3 pos = glm::vec3((x - totalRow/2.0f)*xDisp, (y - totalCol/2.0f)*yDisp, 0.5f);
//            pointLight.GetComponent<TransformComponent>().position = pos;
//            pointLight.AddComponent<LightComponent>(LightComponent::LightType::POINT);
//            //pointLight.AddComponent<LightComponent>(LightComponent::LightType::DIRECTIONAL);
//        }
//    }
//
    totalRow = 7;
    totalCol = 7;
    xDisp = 2.5f;
    yDisp = 2.5f;
    for (int y = 0; y <= totalCol; ++y)
    {
        for (int x = 0; x <= totalRow; ++x)
        {
            std::string name = "Sphere (" + std::to_string(x) + ", " + std::to_string(y) + ")";
            auto sphere = CreateEntity(name);

            glm::vec3 pos = glm::vec3((x - totalRow/2.0f)*xDisp, (y - totalCol/2.0f)*yDisp, 0.0f);
            sphere.GetComponent<TransformComponent>().position = pos;
            sphere.GetComponent<TransformComponent>().rotation = glm::vec3(-90.0f, 0.0f, 0.0f);
            sphere.AddComponent<RigidbodyComponent>();
            sphere.AddComponent<ColliderComponent>(ColliderComponent::Shape::Sphere);
            sphere.AddComponent<MeshComponent>(MeshComponent::Geometry::SPHERE);

            // Differ the material values
            auto& mesh = sphere.GetComponent<MeshComponent>().mesh;
            //mesh->albedo = glm::vec3((float)x/totalRow*0.01f + 0.5f, (float)y/totalCol*0.01f + 0.5f, 1.0f);
            //mesh->albedo = glm::vec3(0.05f, 0.1f, 0.8f);
            mesh->albedo = glm::vec3(0.0f, 1.0f, 0.1f);
            mesh->albedo = glm::vec3(1.0f);
            mesh->metallic = std::max((float)y / (float)totalCol, 0.01f);
            mesh->roughness = std::max((float)x / (float)totalRow, 0.01f);
        }
    }

    /*
    auto testFloor = CreateEntity("Test Floor");
    testCube.GetComponent<TransformComponent>().position = glm::vec3(0.0f, -10.0f, 0.0f);
    testCube.GetComponent<TransformComponent>().scale = glm::vec3(10.0f, 1.0f, 10.0f);
    testCube.AddComponent<RigidbodyComponent>();
    testCube.AddComponent<ColliderComponent>(ColliderComponent::Shape::Box);
    testCube.AddComponent<MeshComponent>(MeshComponent::Geometry::CUBE);
    */

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

        //"../Assets/Textures/Skyboxes/Lake/right.jpg",
        //"../Assets/Textures/Skyboxes/Lake/left.jpg",
        //"../Assets/Textures/Skyboxes/Lake/top.jpg",
        //"../Assets/Textures/Skyboxes/Lake/bottom.jpg",
        //"../Assets/Textures/Skyboxes/Lake/front.jpg",
        //"../Assets/Textures/Skyboxes/Lake/back.jpg"
    };
    skybox->Load(skyboxTextures);
}

void Scene::OnUpdate(float dt)
{
    // Update all entities
    // TODO Based on the lua script attached
    // also if theres a rigidbody attached
    // Test with native c++ script
    registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
    {
        if (!nsc.instance)
        {
            nsc.InstantiateFunction();
            nsc.instance->entity = Entity(entity, this);
            nsc.OnCreateFunction(nsc.instance);
        }

        nsc.OnUpdateFunction(nsc.instance, dt);
    });


    PhysicsManager::OnUpdate(dt);

    // Update rigidbodies
    {
        auto group = registry.group<RigidbodyComponent>(entt::get<TransformComponent>);
        for (auto entity : group)
        {
            auto [transformComponent, rigidbodyComponent] = group.get<TransformComponent, RigidbodyComponent>(entity);

            PhysicsManager::UpdateEntity(rigidbodyComponent.rigidbody, transformComponent);
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
    uboCamera->SetData((void*)glm::value_ptr(camera.pos), 3*sizeof(glm::mat4), sizeof(glm::vec4));
    uboCamera->Unbind();
}

// Render scene from perspective of editor camera
void Scene::OnRenderEditor(float dt, const Camera& editorCamera, bool isPlaying)
{
    SubmitCamera(editorCamera);

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
                particleSystemComponent.ps->OnUpdate(dt, editorCamera.pos);
                particleSystemComponent.ps->Draw(transformComponent);
            }
        }
    }

    // Render sprites
    {
        auto group = registry.group<SpriteComponent>(entt::get<TransformComponent>);
        for (auto entity : group)
        {
            auto [transformComponent, spriteComponent] = group.get<TransformComponent, SpriteComponent>(entity);

            Renderer2D::SubmitQuad(transformComponent, spriteComponent.sprite, spriteComponent.color);
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

                if (isPlaying)
                {
                    //Renderer::SubmitMesh(meshComponent, transformComponent.runtimeTransform);
                }
                else
                    Renderer::SubmitMesh(meshComponent, transformComponent, GetSkybox()->GetTextureCube());
            }
        }
    }
}

// Render scene from perspective of editor camera
void Scene::OnRenderRuntime(float dt)
{
    // Search for the first object in our scene that contains both camera and transform components
    glm::vec3 cameraPos;
    auto group = registry.group<CameraComponent>(entt::get<TransformComponent>);
    for (auto entity : group)
    {
        auto [transformComponent, cameraComponent] = group.get<TransformComponent, CameraComponent>(entity);

        auto gameCamera = cameraComponent.camera;
        cameraPos = transformComponent.position;
        gameCamera->pos = cameraPos;
        gameCamera->RecalculateViewMatrix(transformComponent.position, transformComponent.rotation);
        gameCamera->SetProjection();
        // TODO change function signature to use camera ptr instead of constantly dereferencing
        SubmitCamera(*gameCamera);
        Renderer::BeginScene(*gameCamera);
        break;
    }


    //OnRender(dt);

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
                particleSystemComponent.ps->OnUpdate(dt, cameraPos);
                particleSystemComponent.ps->Draw(transformComponent);
            }
        }
    }

    // Render sprites
    {
        auto group = registry.group<SpriteComponent>(entt::get<TransformComponent>);
        for (auto entity : group)
        {
            auto [transformComponent, spriteComponent] = group.get<TransformComponent, SpriteComponent>(entity);

            Renderer2D::SubmitQuad(transformComponent.runtimeTransform, spriteComponent.sprite, spriteComponent.color);
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

                //Renderer::SubmitMesh(meshComponent, transformComponent.runtimeTransform);
            }
        }
    }

}

void Scene::OnRender(float dt)
{

}

void Scene::SubmitLights()
{
    auto group = registry.group<LightComponent>(entt::get<TransformComponent>);
    uint32_t numLights = 0;

    uboLights->Bind();

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

void Scene::SetSkybox(SharedPtr<Skybox> newSkybox)
{
    skybox = newSkybox;
}

Entity Scene::CreateEntity(const std::string& name, uint64_t ID)
{
    auto entity = Entity(registry.create(), this);

    // Default components all entities should have
    entity.AddComponent<IDComponent>(ID);
    entity.AddComponent<TagComponent>(name);
    entity.AddComponent<TransformComponent>();

    entityMap[ID] = entity;

    return entity;
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
    //registry.destroy(entity);
}


