#include "Scene/Scene.h"
#include "Core/Random.h"
#include <imgui.h>

#include "Renderer/SceneRenderer.h"
#include "Physics/PhysicsManager.h"
#include "Scripting/CameraController.h"

Scene::Scene(bool isNewScene)
{
    SceneRenderer::Init();

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
    mainCamera.GetComponent<CameraComponent>().isMain = true;
    mainCamera.AddComponent<NativeScriptComponent>().Bind<Script::CameraController>();

    auto directionalLight = CreateEntity("Directional Light");
    directionalLight.GetComponent<TransformComponent>().position = glm::vec3(0.0f, 5.0f, 2.0f);
    directionalLight.GetComponent<TransformComponent>().rotation = glm::vec3(120.0f, 0.0f, 0.0f);
    directionalLight.AddComponent<LightComponent>();

    auto model = CreateEntity("Model");
    model.AddComponent<MeshComponent>(MeshComponent::Geometry::CUBE);
    model.GetComponent<TransformComponent>().rotation = glm::vec3(-90.0f, 0.0f, 0.0f);
    auto& material = model.GetComponent<MeshComponent>().material;
    std::string directory = "../Assets/Textures/pbr/wall/";
    //std::string directory = "../Assets/Textures/pbr/gold/";
    material->albedoMap           = Texture2D::Create(directory + "albedo.png");
    material->specularMap         = Texture2D::Create(directory + "metallic.png");
    material->normalMap           = Texture2D::Create(directory + "normal.png");
    material->roughnessMap        = Texture2D::Create(directory + "roughness.png");
    material->ambientOcclusionMap = Texture2D::Create(directory + "ao.png");

    {
        auto pointLight = CreateEntity("Point Light");
        glm::vec3 pos = glm::vec3(0.0f, 1.5f, 0.0f);
        pointLight.GetComponent<TransformComponent>().position = pos;
        pointLight.AddComponent<LightComponent>(LightComponent::LightType::POINT);
        glm::vec4 color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
        pointLight.GetComponent<LightComponent>().color = color;
    }
    {
        auto pointLight = CreateEntity("Point Light");
        glm::vec3 pos = glm::vec3(1.5f, 0.0f, 0.0f);
        pointLight.GetComponent<TransformComponent>().position = pos;
        pointLight.AddComponent<LightComponent>(LightComponent::LightType::POINT);
        glm::vec4 color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
        pointLight.GetComponent<LightComponent>().color = color;
    }
    {
        auto pointLight = CreateEntity("Point Light");
        glm::vec3 pos = glm::vec3(0.0f, 0.0f, 1.5f);
        pointLight.GetComponent<TransformComponent>().position = pos;
        pointLight.AddComponent<LightComponent>(LightComponent::LightType::POINT);
        glm::vec4 color = glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);
        pointLight.GetComponent<LightComponent>().color = color;
    }

    // Setup default skybox
    skybox = CreateUniquePtr<Skybox>();
    std::vector<std::string> skyboxTextures =
    {
        //"../Assets/Textures/Skyboxes/blue/right.png",
        //"../Assets/Textures/Skyboxes/blue/left.png",
        //"../Assets/Textures/Skyboxes/blue/top.png",
        //"../Assets/Textures/Skyboxes/blue/bottom.png",
        //"../Assets/Textures/Skyboxes/blue/front.png",
        //"../Assets/Textures/Skyboxes/blue/back.png"

        "../Assets/Textures/Skyboxes/Lake/right.jpg",
        "../Assets/Textures/Skyboxes/Lake/left.jpg",
        "../Assets/Textures/Skyboxes/Lake/top.jpg",
        "../Assets/Textures/Skyboxes/Lake/bottom.jpg",
        "../Assets/Textures/Skyboxes/Lake/front.jpg",
        "../Assets/Textures/Skyboxes/Lake/back.jpg"
    };
    skybox->Load(skyboxTextures);
}

void Scene::OnUpdate(float dt)
{
    // Update with Native C++ scripts
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
    auto group = registry.group<RigidbodyComponent>(entt::get<TransformComponent>);
    for (auto entity : group)
    {
        auto [transformComponent, rigidbodyComponent] = group.get<TransformComponent, RigidbodyComponent>(entity);

        PhysicsManager::UpdateEntity(rigidbodyComponent.rigidbody, transformComponent);
    }

    /*
    // Get the main game camera
    Camera gameCamera;
    auto group = registry.group<CameraComponent>(entt::get<TransformComponent>);
    for (auto entity : group)
    {
        auto [transformComponent, cameraComponent] = group.get<TransformComponent, CameraComponent>(entity);

        gameCamera = *cameraComponent.camera;
        gameCamera.RecalculateViewMatrix(transformComponent.position, transformComponent.rotation);
        gameCamera.SetProjection();
        gameCamera.pos = transformComponent.position;
        break;
    }

    // Render scene through game camera
    SceneRenderer::BeginScene(this, gameCamera);
    Scene::OnRender(dt, gameCamera);
    SceneRenderer::EndScene();

    // Render scene through editor camera
    SceneRenderer::BeginScene(this, editorCamera);
    Scene::OnRender(dt, editorCamera);
    SceneRenderer::EndScene();
    */
}

// Render scene from perspective of editor camera
void Scene::OnRenderEditor(float dt, const Camera& editorCamera)
{

    // Render particles first since they're transparent
//    {
//        auto group = registry.group<ParticleSystemComponent>(entt::get<TransformComponent>);
//        for (auto entity : group)
//        {
//            auto [transformComponent, particleSystemComponent] = group.get<TransformComponent, ParticleSystemComponent>(entity);
//            if (particleSystemComponent.ps)
//            {
//                particleSystemComponent.ps->position = transformComponent.position;
//                // TODO have this transparency ordering check in renderer instead of PS
//                // or swap to OIT eventually
//                //particleSystemComponent.ps->OnUpdate(dt, editorCamera.pos);
//                particleSystemComponent.ps->Draw(transformComponent);
//            }
//        }
//    }
//
//    // Render sprites
//    {
//        auto group = registry.group<SpriteComponent>(entt::get<TransformComponent>);
//        for (auto entity : group)
//        {
//            auto [transformComponent, spriteComponent] = group.get<TransformComponent, SpriteComponent>(entity);
//
//            Renderer2D::SubmitQuad(transformComponent, spriteComponent.sprite, spriteComponent.color);
//        }
//    }

    // Render opaque meshes
    // Only render objects that have mesh components
    auto group = registry.group<MeshComponent>(entt::get<TransformComponent>);
    for (auto entity : group)
    {
        auto [transformComponent, meshComponent] = group.get<TransformComponent, MeshComponent>(entity);
        if (meshComponent.mesh)
        {
            //meshComponent.mesh->OnUpdate(dt);

            SceneRenderer::SubmitMesh(meshComponent, transformComponent, meshComponent.material);
        }
    }
}

/*
// Render scene from perspective of editor camera
void Scene::OnRenderRuntime(float dt)
{
    // Search for the first object in our scene that contains both camera and transform components
    glm::vec3 cameraPos;
    //OnRender(dt);

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
*/

void Scene::OnEvent(Event& e)
{
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


