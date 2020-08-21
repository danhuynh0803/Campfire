#include "Scene/Scene.h"
#include "Core/Random.h"
#include <imgui.h>

#include "Renderer/SceneRenderer.h"
#include "Renderer/Renderer2D.h"
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

    {
        auto model = CreateEntity("Model1");
        model.AddComponent<MeshComponent>(MeshComponent::Geometry::CUBE);
        model.GetComponent<TransformComponent>().position = glm::vec3(-1.0f, 0.0f, 0.0f);
        // TODO: rb runtime transform isn't submitting correct rotation
        model.GetComponent<TransformComponent>().rotation = glm::vec3(-90.0f, 0.0f, 0.0f);
        model.AddComponent<RigidbodyComponent>();
        model.AddComponent<ColliderComponent>(ColliderComponent::Shape::Box);
        auto& material = model.GetComponent<MeshComponent>().material;
        std::string directory = "../Assets/Textures/pbr/wall/";
        material->albedoMap           = Texture2D::Create(directory + "albedo.png");
        material->specularMap         = Texture2D::Create(directory + "metallic.png");
        material->normalMap           = Texture2D::Create(directory + "normal.png");
        material->roughnessMap        = Texture2D::Create(directory + "roughness.png");
        material->ambientOcclusionMap = Texture2D::Create(directory + "ao.png");
    }

    {
        auto model = CreateEntity("Model2");
        model.AddComponent<MeshComponent>(MeshComponent::Geometry::SPHERE);
        model.GetComponent<TransformComponent>().position = glm::vec3(1.0f, 0.0f, 0.0f);
        model.GetComponent<TransformComponent>().rotation = glm::vec3(-90.0f, 0.0f, 0.0f);
        model.AddComponent<RigidbodyComponent>();
        model.AddComponent<ColliderComponent>(ColliderComponent::Shape::Sphere);
        auto& material = model.GetComponent<MeshComponent>().material;
        std::string directory = "../Assets/Textures/pbr/wall/";
        material->albedoMap           = Texture2D::Create(directory + "albedo.png");
        material->specularMap         = Texture2D::Create(directory + "metallic.png");
        material->normalMap           = Texture2D::Create(directory + "normal.png");
        material->roughnessMap        = Texture2D::Create(directory + "roughness.png");
        material->ambientOcclusionMap = Texture2D::Create(directory + "ao.png");
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
}

// Render scene from perspective of editor camera
void Scene::OnRender(float dt, const Camera& camera, bool isPlaying)
{
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
                particleSystemComponent.ps->OnUpdate(dt, camera.pos);
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
                SceneRenderer::SubmitMesh(meshComponent, transformComponent.runtimeTransform, meshComponent.material);
            }
            else
            {
                SceneRenderer::SubmitMesh(meshComponent, transformComponent, meshComponent.material);
            }
        }
    }
}

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


