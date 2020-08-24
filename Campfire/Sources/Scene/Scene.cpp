#include "Scene/Scene.h"
#include "Core/Random.h"
#include <imgui.h>

#include "Renderer/SceneRenderer.h"
#include "Renderer/Renderer2D.h"
#include "Physics/PhysicsManager.h"
#include "Scripting/CameraController.h"
#include "Scripting/PlayerController.h"

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
    */
    auto mainCamera = CreateEntity("Camera");
    mainCamera.GetComponent<TransformComponent>().position = glm::vec3(0.0f, 0.0f, 10.0f);
    mainCamera.AddComponent<CameraComponent>();
    mainCamera.GetComponent<CameraComponent>().isMain = true;
    mainCamera.AddComponent<NativeScriptComponent>().Bind<Script::CameraController>();

    auto directionalLight = CreateEntity("Directional Light");
    directionalLight.GetComponent<TransformComponent>().position = glm::vec3(0.0f, 5.0f, 2.0f);
    directionalLight.GetComponent<TransformComponent>().eulerAngles = glm::vec3(120.0f, 0.0f, 0.0f);
    directionalLight.AddComponent<LightComponent>();

    auto player = CreateEntity("Model1");
    player.AddComponent<MeshComponent>(MeshComponent::Geometry::SPHERE);
    player.GetComponent<TransformComponent>().position = glm::vec3(-1.0f, 0.0f, 0.0f);
    player.GetComponent<TransformComponent>().eulerAngles = glm::vec3(-90.0f, 0.0f, 0.0f);
    player.AddComponent<RigidbodyComponent>();
    player.GetComponent<RigidbodyComponent>().rigidbody->type = Rigidbody::BodyType::KINEMATIC;
    player.AddComponent<ColliderComponent>(ColliderComponent::Shape::Sphere);
    auto& material = player.GetComponent<MeshComponent>().material;
    std::string directory = "../Assets/Textures/pbr/wall/";
    material->albedoMap           = Texture2D::Create(directory + "albedo.png");
    material->specularMap         = Texture2D::Create(directory + "metallic.png");
    material->normalMap           = Texture2D::Create(directory + "normal.png");
    material->roughnessMap        = Texture2D::Create(directory + "roughness.png");
    material->ambientOcclusionMap = Texture2D::Create(directory + "ao.png");

    player.AddComponent<NativeScriptComponent>().Bind<Script::PlayerController>();



    auto floor = CreateEntity("Floor");
    floor.AddComponent<MeshComponent>(MeshComponent::Geometry::CUBE);
    floor.GetComponent<TransformComponent>().position = glm::vec3(0.0f, -5.0f, 0.0f);
    floor.GetComponent<TransformComponent>().scale = glm::vec3(10.0f, 0.2f, 10.0f);
    floor.GetComponent<TransformComponent>().eulerAngles = glm::vec3(0.0f, 0.0f, 0.0f);
    //floor.AddComponent<RigidbodyComponent>();
    floor.AddComponent<ColliderComponent>(ColliderComponent::Shape::Box);
    auto& material1 = floor.GetComponent<MeshComponent>().material;
    std::string directory1 = "../Assets/Textures/pbr/snow/";
    material1->albedoMap           = Texture2D::Create(directory1 + "albedo.png");
    material1->specularMap         = Texture2D::Create(directory1 + "metallic.png");
    material1->normalMap           = Texture2D::Create(directory1 + "normal.png");
    material1->roughnessMap        = Texture2D::Create(directory1 + "roughness.png");
    material1->ambientOcclusionMap = Texture2D::Create(directory1 + "ao.png");

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

template<typename T>
static void CopyComponent(entt::registry& dstRegistry, entt::registry& srcRegistry, const std::map<uint64_t, entt::entity>& enttMap)
{
    auto components = srcRegistry.view<T>();
    for (auto srcEntity : components)
    {
        entt::entity destEntity = enttMap.at(srcRegistry.get<IDComponent>(srcEntity).ID);

        auto& srcComponent = srcRegistry.get<T>(srcEntity);
        auto& destComponent = dstRegistry.emplace_or_replace<T>(destEntity, srcComponent);
    }
}

template<typename T>
static void CopyComponentIfExists(entt::entity dst, entt::entity src, entt::registry& registry)
{
    if (registry.has<T>(src))
    {
        auto& srcComponent = registry.get<T>(src);
        registry.emplace_or_replace<T>(dst, srcComponent);
    }
}

void Scene::DeepCopy(const SharedPtr<Scene>& other)
{
    // First clear out current scene contents
    Clear();

    // Copy skybox
    skybox = other->skybox;

    // Copy all entities
    std::map<uint64_t, entt::entity> enttMap;
    auto idComps = other->registry.view<IDComponent>();
    for (auto entity : idComps)
    {
        uint64_t id = other->registry.get<IDComponent>(entity).ID;
        Entity e = CreateEntity("", id);
        enttMap[id] = e;
    }

    if (!enttMap.empty())
    {
        CopyComponent<TagComponent>(registry, other->registry, enttMap);
        CopyComponent<TransformComponent>(registry, other->registry, enttMap);
        CopyComponent<MeshComponent>(registry, other->registry, enttMap);
        CopyComponent<SpriteComponent>(registry, other->registry, enttMap);
        CopyComponent<LightComponent>(registry, other->registry, enttMap);
        CopyComponent<RigidbodyComponent>(registry, other->registry, enttMap);
        CopyComponent<ColliderComponent>(registry, other->registry, enttMap);
        CopyComponent<CameraComponent>(registry, other->registry, enttMap);
        CopyComponent<ParticleSystemComponent>(registry, other->registry, enttMap);
        CopyComponent<NativeScriptComponent>(registry, other->registry, enttMap);
    }
    // TODO
    //CopyComponent<AudioComponent>(registry, other->registry, enttMap);
}

void Scene::OnStart()
{
    // Submit all entities with rbs to Physics
    PhysicsManager::ClearLists();
    for (auto entityPair : entityMap)
    {
        PhysicsManager::SubmitEntity(entityPair.second);
    }

    // Initialize scripts and run their Start()
    registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
    {
        if (!nsc.instance)
        {
            nsc.instance = nsc.InstantiateScript();
            nsc.instance->entity = Entity(entity, this);
        }
        nsc.instance->Start();
    });
}

void Scene::OnUpdate(float dt)
{
    PhysicsManager::OnUpdate(dt);

    // Update with Native C++ scripts
    registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
    {
        nsc.instance->Update(dt);
    });

    // Update rigidbodies
    auto group = registry.group<RigidbodyComponent>(entt::get<TransformComponent>);
    for (auto entity : group)
    {
        auto [transformComponent, rigidbodyComponent] = group.get<TransformComponent, RigidbodyComponent>(entity);

        PhysicsManager::UpdateEntity(rigidbodyComponent.rigidbody, transformComponent);
    }
}

// Render scene from perspective of editor camera
void Scene::OnRender(float dt, const Camera& camera)
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
            SceneRenderer::SubmitMesh(meshComponent, transformComponent, meshComponent.material);
        }
    }
}

void Scene::OnEvent(Event& e)
{
    // TODO do raycast on scene to get selected entity
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
    if (entity.HasComponent<NativeScriptComponent>())
    {
        // TODO rewrite to destroy without passing itself
        entity.GetComponent<NativeScriptComponent>().DestroyScript(&entity.GetComponent<NativeScriptComponent>());
    }

    if (entity.HasComponent<RigidbodyComponent>())
    {
        PhysicsManager::RemoveEntity(entity.GetComponent<RigidbodyComponent>().rigidbody->GetBulletRigidbody());
    }

    entityMap.erase(entity.GetComponent<IDComponent>());
    registry.destroy(entity);
}


