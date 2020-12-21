#include <imgui.h>
#include <fstream>
#include <Tracy.hpp>

#include "Core/Random.h"
#include "Core/ResourceManager.h"

#include "Scene/Entity.h"
#include "Scene/Scene.h"
#include "Scene/SceneManager.h"

#include "Renderer/SceneRenderer.h"
#include "Renderer/Renderer2D.h"
#include "Physics/PhysicsManager.h"
#include "Particles/ParticleSystem.h"
#include "Scripting/CameraController.h"
#include "Scripting/PlayerController.h"

// Should be moved as a subsystem
#include "Audio/AudioSystem.h"
#include "Scene/Component.h"
#include "Scene/Skybox.h"
#include "Scripting/LuaScript.h"

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
    //auto mainCamera = CreateEntity("Camera", false); // false for not setting it as a root object in entityMap
    mainCamera.GetComponent<TransformComponent>().position = glm::vec3(0.0f, 0.0f, 10.0f);
    mainCamera.AddComponent<CameraComponent>();
    mainCamera.AddComponent<NativeScriptComponent>().Bind<NativeScript::CameraController>();
    mainCamera.AddComponent<AudioComponent>();
    mainCamera.GetComponent<AudioComponent>().audioSource->clipPath = ASSETS + "/Audio/test.wav";
    mainCamera.GetComponent<AudioComponent>().audioSource->playOnAwake = true;

    auto directionalLight = CreateEntity("Directional Light");
    directionalLight.GetComponent<TransformComponent>().position = glm::vec3(0.0f, 5.0f, 2.0f);
    directionalLight.GetComponent<TransformComponent>().euler = glm::vec3(120.0f, 0.0f, 0.0f);
    directionalLight.AddComponent<LightComponent>();

    {
        auto player = CreateEntity("Player");
        player.AddComponent<MeshComponent>(MeshComponent::Geometry::SPHERE);
        player.GetComponent<TransformComponent>().position = glm::vec3(-1.0f, 0.0f, 0.0f);
        player.AddComponent<RigidbodyComponent>();
        player.GetComponent<RigidbodyComponent>().rigidbody->type = Rigidbody::BodyType::DYNAMIC;
        player.GetComponent<RigidbodyComponent>().rigidbody->useGravity = false;
        player.AddComponent<ScriptComponent>().template Bind<LuaScript>();
        player.GetComponent<ScriptComponent>().filepath = ASSETS + "/Scripts/test.lua";
        //player.GetComponent<TransformComponent>().eulerAngles = glm::vec3(-90.0f, 0.0f, 0.0f);
        auto& colliders = player.GetComponent<Colliders>().list;
        colliders.emplace_back(Collider::Create(Collider::Shape::SPHERE));
        colliders.at(0)->isTrigger = true;
        //player.AddComponent<TriggerComponent>();
        //player.AddComponent<AudioComponent>();
        //player.GetComponent<AudioComponent>().audioSource->clipPath = ASSETS + "Audio/metal.mp3";
        //player.AddChild(mainCamera);
    }

    {
        auto cube = CreateEntity("Cube");
        cube.AddComponent<MeshComponent>(MeshComponent::Geometry::CUBE);
        cube.GetComponent<TransformComponent>().position = glm::vec3(-1.0f, 5.0f, 0.0f);
        cube.GetComponent<TransformComponent>().euler = glm::vec3(-90.0f, 0.0f, 0.0f);
        cube.AddComponent<RigidbodyComponent>();
        cube.GetComponent<RigidbodyComponent>().rigidbody->type = Rigidbody::BodyType::DYNAMIC;
        auto& colliders = cube.GetComponent<Colliders>().list;
        colliders.emplace_back(Collider::Create(Collider::Shape::BOX));
    }

    {
        auto floor = CreateEntity("Floor");
        //floor.AddComponent<MeshComponent>(MeshComponent::Geometry::CUBE);
        floor.GetComponent<TransformComponent>().position = glm::vec3(0.0f, -10.0f, 0.0f);
        floor.GetComponent<TransformComponent>().scale = glm::vec3(20.0f, 0.2f, 20.0f);
        floor.GetComponent<TransformComponent>().euler = glm::vec3(0.0f, 0.0f, 0.0f);
        floor.AddComponent<RigidbodyComponent>();
        floor.GetComponent<RigidbodyComponent>().rigidbody->type = Rigidbody::BodyType::STATIC;
        //floor.AddComponent<ColliderComponent>(ColliderComponent::Shape::Box);
        auto& colliders = floor.GetComponent<Colliders>().list;
        auto collider = Collider::Create(Collider::Shape::BOX);
        collider->isTrigger = true;
        colliders.emplace_back(collider);
        floor.AddComponent<ScriptComponent>().template Bind<LuaScript>();
        floor.GetComponent<ScriptComponent>().filepath = ASSETS + "/Scripts/killborder.lua";
    }

    // TODO replace with HDR skybox
    // Setup default skybox
    skybox = CreateUniquePtr<Skybox>();
    std::vector<std::string> skyboxTextures =
    {
        ASSETS + "/Textures/Skyboxes/Lake/right.jpg",
        ASSETS + "/Textures/Skyboxes/Lake/left.jpg",
        ASSETS + "/Textures/Skyboxes/Lake/top.jpg",
        ASSETS + "/Textures/Skyboxes/Lake/bottom.jpg",
        ASSETS + "/Textures/Skyboxes/Lake/front.jpg",
        ASSETS + "/Textures/Skyboxes/Lake/back.jpg"
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

void Scene::CopyFrom(const SharedPtr<Scene>& srcScene)
{
    // First clear out current scene contents
    Clear();

    // Copy skybox
    skybox = srcScene->skybox;

    // Copy all entities
    std::map<uint64_t, entt::entity> enttMap;
    auto idComps = srcScene->registry.view<IDComponent>();
    for(auto rit = idComps.rbegin(); rit != idComps.rend(); ++rit)
    {
        uint64_t id = srcScene->registry.get<IDComponent>(*rit).ID;
        Entity e = CreateEntity("");
        e.GetComponent<IDComponent>().ID = id;
        //LOG_INFO(e.GetComponent<IDComponent>().ID);
        enttMap[id] = *rit;
    }

    if (!enttMap.empty())
    {
        CopyComponent<ActiveComponent>(registry, srcScene->registry, enttMap);
        CopyComponent<TagComponent>(registry, srcScene->registry, enttMap);
        CopyComponent<TransformComponent>(registry, srcScene->registry, enttMap);
        CopyComponent<MeshComponent>(registry, srcScene->registry, enttMap);
        CopyComponent<SpriteComponent>(registry, srcScene->registry, enttMap);
        CopyComponent<LightComponent>(registry, srcScene->registry, enttMap);
        CopyComponent<RigidbodyComponent>(registry, srcScene->registry, enttMap);
        CopyComponent<TriggerComponent>(registry, srcScene->registry, enttMap);
        CopyComponent<Colliders>(registry, srcScene->registry, enttMap);
        CopyComponent<TriggerComponent>(registry, srcScene->registry, enttMap);
        CopyComponent<CameraComponent>(registry, srcScene->registry, enttMap);
        CopyComponent<ParticleSystemComponent>(registry, srcScene->registry, enttMap);
        CopyComponent<ScriptComponent>(registry, srcScene->registry, enttMap);
        CopyComponent<NativeScriptComponent>(registry, srcScene->registry, enttMap);
        CopyComponent<AudioComponent>(registry, srcScene->registry, enttMap);
        CopyComponent<TextComponent>(registry, srcScene->registry, enttMap);
        //CopyComponent<RelationshipComponent>(registry, srcScene->registry, enttMap);
    }
}

void Scene::OnStart()
{
    // TODO
    // Initialize prefabs into memory
    auto prefabs = FileSystem::GetAllFiles(ASSETS.c_str(), ".prefab");
    for (auto path : prefabs)
    {
        std::ifstream input(path.string());
        json eJson;
        input >> eJson;
        ResourceManager::mPrefabMap[path.filename().string()] = eJson;
    }

    // Submit all entities with rbs to Physics
    // FIXME: physics needs to be submitted before start
    // in order to make use of RB functions in Start().
    // But if an entity is instantiated in Start(), then
    // it's physics wont be active since it wasn't submitted
    // into the list
    PhysicsManager::ClearLists();
    auto group = registry.group<Colliders>(entt::get<TransformComponent>);
    for (auto handle : group)
    {
        Entity entity(handle, this);
        PhysicsManager::SubmitEntity(entity);
    }

    // Play all OnAwake sounds
    registry.view<AudioComponent>().each([=](auto entity, auto &audioComp)
    {
        auto audioSource = audioComp.audioSource;
        if (audioSource->playOnAwake)
        {
            audioSource->Play();
        }
    });

    registry.view<ScriptComponent>().each([=](auto entity, auto& sc)
    {
        if (sc.isActive && !sc.filepath.empty())
        {
            if (!sc.instance)
            {
                sc.instance = sc.InstantiateScript();
                sc.instance->entity = Entity(entity, this);
                sc.instance->filepath = sc.filepath;
                sc.instance->runUpdate = sc.runUpdate;
                sc.instance->runOnTriggerEnter = sc.runOnTriggerEnter;
                sc.instance->runOnTriggerStay = sc.runOnTriggerStay;
                sc.instance->runOnTriggerExit = sc.runOnTriggerExit;
            }
            sc.instance->Start();
        }
    });

    // Initialize scripts and run their Start()
    //registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
    //{
    //    if (!nsc.instance)
    //    {
    //        nsc.instance = nsc.InstantiateScript();
    //        nsc.instance->entity = Entity(entity, this);
    //    }
    //    nsc.instance->Start();
    //});


}

void Scene::OnStop()
{
    AudioSystem::StopAllChannels();
}

void Scene::OnUpdate(float dt)
{
    // Remove all entities marked for deletion
    {
        registry.view<Destroy>().each([=](auto handle, auto& destroy)
        {
            Entity entity(handle, this);
            RemoveEntity(entity);
        });
    }

    AudioSystem::OnUpdate(dt);
    PhysicsManager::OnUpdate(dt);

    // Update rigidbodies
    {
        ZoneScopedN("UpdateRigidbodies");
        auto group = registry.group<RigidbodyComponent>(entt::get<TransformComponent>);
        for (auto entity : group)
        {
            auto [transformComponent, rigidbodyComponent] = group.get<TransformComponent, RigidbodyComponent>(entity);

            if (rigidbodyComponent.isActive)
                PhysicsManager::UpdateEntity(rigidbodyComponent, transformComponent);
        }
    }

    // Update triggers
    std::vector<entt::entity> overlappingEntities;
    {
        ZoneScopedN("UpdateTriggers");
        auto triggerGroup = registry.group<TriggerComponent>(entt::get<TransformComponent>);
        for (auto entity : triggerGroup)
        {
            auto [transformComponent, triggerComponent] = triggerGroup.get<TransformComponent, TriggerComponent>(entity);

            overlappingEntities = PhysicsManager::UpdateTrigger(triggerComponent, transformComponent);
        }
    }

    // Lua script update
    {
        ZoneScopedN("LuaUpdateScripts");
        registry.view<ScriptComponent>().each([=](auto entity, auto& sc)
        {
            if (!sc.isActive || sc.filepath.empty() || sc.instance->hasSynataxError) return;//for std::function

            //bool runUpdate, runOnTriggerEnter, runOnTriggerStay, runOnTriggerExit;
            //lua_State* L = luaL_newstate();
            //if (luaL_dofile(L, sc.filepath.c_str()) == LUA_OK)//check syntax error
            //{
            //    runUpdate = (lua_getglobal(L, "Update")) ? sc.runUpdate : false;
            //    runOnTriggerEnter = (lua_getglobal(L, "OnTriggerEnter")) ? sc.runOnTriggerEnter : false;
            //    runOnTriggerStay = (lua_getglobal(L, "OnTriggerStay")) ? sc.runOnTriggerStay : false;
            //    runOnTriggerExit = (lua_getglobal(L, "OnTriggerStay")) ? sc.runOnTriggerExit : false;
            //}

            if (sc.runUpdate) sc.instance->Update(dt);

            Entity thisEntity = sc.instance->entity;
            if (thisEntity.HasComponent<TriggerComponent>())
            {
                SharedPtr<Trigger> trigger = thisEntity.GetComponent<TriggerComponent>();
                for (auto enterEntity : trigger->overlapEnterList)
                {
                    Entity other(enterEntity, this);
                    // Don't have the trigger apply on ourselves
                    // since the trigger and rb will always be colliding
                    if (other && enterEntity != sc.instance->entity && sc.runOnTriggerEnter)
                    {
                        LOG_INFO("{0}, {1}", other.GetComponent<TagComponent>().tag, static_cast<uint64_t>(enterEntity));
                        sc.instance->OnTriggerEnter(other);
                    }
                }

                for (auto stayEntity : overlappingEntities)
                {
                    Entity other(stayEntity, this);
                    if (other && stayEntity != sc.instance->entity && sc.runOnTriggerStay)
                    {
                        sc.instance->OnTriggerStay(other);
                    }
                }

                for (auto exitEntity : trigger->overlapExitList)
                {
                    Entity other(exitEntity, this);
                    if (other && exitEntity != sc.instance->entity && sc.runOnTriggerExit)
                    {
                        sc.instance->OnTriggerExit(other);
                    }
                }
            }
        });
    }

    // Update with Native C++ scripts
    //{
    //    //ZoneScopedN("UpdateNativeScripts");
    //    registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
    //    {
    //        nsc.instance->Update(dt);

    //        Entity thisEntity = nsc.instance->entity;
    //        if (thisEntity.HasComponent<TriggerComponent>())
    //        {
    //            SharedPtr<Trigger> trigger = thisEntity.GetComponent<TriggerComponent>();
    //            for (auto enterEntity : trigger->overlapEnterList)
    //            {
    //                Entity other(enterEntity, this);
    //                // Don't have the trigger apply on ourselves
    //                // since the trigger and rb will always be colliding
    //                if (enterEntity != nsc.instance->entity)
    //                {
    //                    nsc.instance->OnTriggerEnter(other);
    //                }
    //            }

    //            for (auto stayEntity : overlappingEntities)
    //            {
    //                Entity other(stayEntity, this);
    //                if (stayEntity != nsc.instance->entity)
    //                {
    //                    nsc.instance->OnTriggerStay(other);
    //                }
    //            }

    //            for (auto exitEntity : trigger->overlapExitList)
    //            {
    //                Entity other(exitEntity, this);
    //                if (exitEntity != nsc.instance->entity)
    //                {
    //                    nsc.instance->OnTriggerExit(other);
    //                }
    //            }
    //        }
    //    });
    //}
}

// Render scene from perspective of designated camera
void Scene::OnRender(float dt, const Camera& camera)
{
    // Render particles first since they're transparent
    {
        ZoneScopedN("RenderParticles");
        {
            auto group = registry.group<ParticleSystemComponent>(entt::get<TransformComponent>);
            for (auto entity : group)
            {
                auto [transformComponent, particleSystemComponent] = group.get<TransformComponent, ParticleSystemComponent>(entity);
                if (particleSystemComponent.isActive && particleSystemComponent.ps)
                {
                    particleSystemComponent.ps->position = transformComponent.position;
                    // TODO have this transparency ordering check in renderer instead of PS
                    // or swap to OIT eventually
                    particleSystemComponent.ps->OnUpdate(dt, camera.pos);
                    particleSystemComponent.ps->Draw(transformComponent);
                }
            }
        }
    }

    // Render sprites
    {
        ZoneScopedN("RenderSprites");
        {
            auto group = registry.group<SpriteComponent>(entt::get<TransformComponent>);
            for (auto entity : group)
            {
                auto [transformComponent, spriteComponent] = group.get<TransformComponent, SpriteComponent>(entity);

                if (spriteComponent.isActive)
                    Renderer2D::SubmitQuad(transformComponent, spriteComponent.sprite, spriteComponent.color);
            }
        }
    }

    // Render opaque meshes
    {
        ZoneScopedN("RenderMeshes");
        // Only render objects that have mesh components
        auto group = registry.group<MeshComponent>(entt::get<TransformComponent, RelationshipComponent>);
        for (auto entity : group)
        {
            auto [transformComponent, relationshipComponent, meshComponent] = group.get<TransformComponent, RelationshipComponent, MeshComponent>(entity);
            if (meshComponent.isActive && meshComponent.mesh)
            {
                // If entity is a child, then apply parents transform to it
                if (relationshipComponent.parent != entt::null)
                {
                    auto parentTransform = registry.get<TransformComponent>(relationshipComponent.parent);

                    glm::mat4 transform = glm::mat4(1.0f);
                    glm::vec3 position    = transformComponent.position + parentTransform.position;
                    glm::vec3 eulerAngles = transformComponent.euler;
                    glm::vec3 scale = transformComponent.scale * parentTransform.scale;

                    glm::vec3 parentEulerAngles = parentTransform.euler;
                    glm::quat parentRotation = glm::quat(
                        glm::vec3(
                            glm::radians(parentEulerAngles.x),
                            glm::radians(parentEulerAngles.y),
                            glm::radians(parentEulerAngles.z)
                        )
                    );
                    //glm::mat4 parentRotationMat = glm::toMat4(parentRotation);
                    //transform = transform * parentRotationMat;
                    glm::vec3 rotationPosition = parentTransform.position + (parentRotation * (position - parentTransform.position));

                    transform = glm::translate(transform, rotationPosition);

                    glm::quat rotation = glm::quat(
                            glm::vec3(
                                glm::radians(eulerAngles.x),
                                glm::radians(eulerAngles.y),
                                glm::radians(eulerAngles.z)
                                )
                            );
                    glm::mat4 rotationMat = glm::toMat4(rotation);
                    transform = transform * rotationMat;

                    transform = glm::scale(transform, scale);

                    SceneRenderer::SubmitMesh(meshComponent, transform);
                }
                else
                {
                    SceneRenderer::SubmitMesh(meshComponent, transformComponent);
                }
            }
        }
    }

    // Render Text objects
    {
        ZoneScopedN("RenderText");
        // Only render objects that have mesh components
        auto group = registry.group<TextComponent>(entt::get<TransformComponent>);
        for (auto entity : group)
        {
            auto [transformComponent, textComponent] = group.get<TransformComponent, TextComponent>(entity);
            if (textComponent.isActive && textComponent.text)
            {
                textComponent.text->Draw(transformComponent, camera);
            }
        }
    }
}

void Scene::OnEvent(Event& e)
{
}

std::string Scene::GetUniqueTag(const std::string& tag)
{
    // Probably a much better way than doing this
    // But scene hierarchy will usually be small and this is done only once during entity creation.
    // Creating a tagList each time will keep track of any changes that were made to existing tags.
    std::vector<std::string> tagList;
    std::string subTag = tag.substr(0, tag.find_last_of("(")); // Dont include the '(' that comes from duplicate tags
    int count = 1;
    auto view = registry.view<TagComponent>();
    for (auto entity : view)
    {
        tagList.emplace_back(view.get<TagComponent>(entity));
    }

    std::string newTag = subTag;
    while (std::find(tagList.begin(), tagList.end(), newTag) != tagList.end())
    {
        newTag = subTag + "(" + std::to_string(count) + ")";
        count++;
    }

    return newTag;
}

Entity Scene::CreateEntity(const std::string& name, bool isRootEntity)
{
    //static uint64_t ID = 0;
    Entity entity = Entity(registry.create(), this);
    // Default components all entities should have
    uint64_t ID = static_cast<uint64_t>(entt::entity(entity));
    entity.AddComponent<IDComponent>(ID);
    std::string tag = GetUniqueTag(name);
    entity.AddComponent<ActiveComponent>();
    entity.AddComponent<TagComponent>(tag);
    entity.AddComponent<TransformComponent>();
    entity.AddComponent<RelationshipComponent>();
    entity.AddComponent<Colliders>();

    if (isRootEntity)
    {
        entityMap[ID] = entity;
    }

    //ID++;

    return entity;
}

Entity Scene::DuplicateEntity(Entity entity)
{
    std::string tag = entity.GetComponent<TagComponent>();
    Entity newEntity = CreateEntity(tag);

    // FIXME have a list of components to copy to avoid duplicate code
    CopyComponentIfExists<ActiveComponent>(newEntity, entity, registry);
    CopyComponentIfExists<TransformComponent>(newEntity, entity, registry);
    CopyComponentIfExists<MeshComponent>(newEntity, entity, registry);
    CopyComponentIfExists<SpriteComponent>(newEntity, entity, registry);
    CopyComponentIfExists<LightComponent>(newEntity, entity, registry);
    CopyComponentIfExists<RigidbodyComponent>(newEntity, entity, registry);
    CopyComponentIfExists<Colliders>(newEntity, entity, registry);
    CopyComponentIfExists<TriggerComponent>(newEntity, entity, registry);
    CopyComponentIfExists<CameraComponent>(newEntity, entity, registry);
    CopyComponentIfExists<ParticleSystemComponent>(newEntity, entity, registry);
    CopyComponentIfExists<ScriptComponent>(newEntity, entity, registry);
    CopyComponentIfExists<NativeScriptComponent>(newEntity, entity, registry);
    CopyComponentIfExists<AudioComponent>(newEntity, entity, registry);
    CopyComponentIfExists<TextComponent>(newEntity, entity, registry);
    //CopyComponent<RelationshipComponent>(newEntity, entity, registry);

    return newEntity;
}

Entity Scene::FindEntityWithTag(const std::string& tag)
{
    auto view = registry.view<TagComponent>();
    for (auto entity : view)
    {
        std::string currTag = view.get<TagComponent>(entity);
        if (currTag == tag)
        {
            return Entity(entity, this);
        }
    }

    return Entity {};
}

void Scene::RemoveEntity(Entity entity)
{
    if (entity.HasComponent<NativeScriptComponent>())
    {
        // TODO rewrite to destroy without passing itself
        entity.GetComponent<NativeScriptComponent>().DestroyScript(&entity.GetComponent<NativeScriptComponent>());
    }

    if (entity.HasComponent<ScriptComponent>())
    {
        entity.GetComponent<ScriptComponent>().DestroyScript(&entity.GetComponent<ScriptComponent>());
    }

    //if (entity.HasComponent<RigidbodyComponent>() || entity.HasComponent<TriggerComponent>())
    //{
    PhysicsManager::RemoveEntity(entity);
    //}

    // Remove this entity from all overlap lists
    auto view = registry.view<TriggerComponent>();
    for (auto handle : view)
    {
        auto trigger = view.get<TriggerComponent>(handle).trigger;
        trigger->RemoveEntityHandle(entity);
    }

    auto it = entityMap.find(entity.GetComponent<IDComponent>());
    if (it != entityMap.end())
    {
        entityMap.erase(it);
    }
    registry.destroy(entity);
}
