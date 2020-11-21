#include "Scene/SceneManager.h"
#include "Scene/Scene.h"
#include "Scene/Component.h"
#include "Scene/Skybox.h"
#include "Core/Base.h"
#include <cstdio>
#include <iomanip>
#include <fstream>
#include <nlohmann/json.hpp>
#include <string>
#include "Scripting/LuaScript.h"

std::string SceneManager::activeScenePath;
std::string SceneManager::activeSceneName;
SharedPtr<Scene> SceneManager::mScene = nullptr;

SharedPtr<Scene> SceneManager::LoadNewScene()
{
    activeScenePath.clear();
    activeSceneName.clear();
    mScene = CreateSharedPtr<Scene>();
    return mScene;
}

SharedPtr<Scene> SceneManager::LoadScene(const std::string& loadPath)
{
    std::ifstream input(loadPath);
    json sceneJson;
    input >> sceneJson;

    activeScenePath.assign(loadPath);
    activeSceneName = activeScenePath.substr(
        activeScenePath.find_last_of('/')+1
    );

    LOG_TRACE("Loading SCENE file: {0}", loadPath);

    // TODO launch on separate thread

    mScene = CreateSharedPtr<Scene>(false);
    SharedPtr<Skybox> skybox = CreateSharedPtr<Skybox>();
    std::vector<std::string> skyboxFiles;
    for (auto& element : sceneJson["Environment"]["skybox"])
    {
        skyboxFiles.emplace_back(element.get<std::string>());
    }
    skybox->Load(skyboxFiles);
    mScene->skybox = skybox;

    // Iterate over entities list
    if (sceneJson.contains("Entities"))
    {
        for (auto& eJson : sceneJson["Entities"])
        {
            DeserializeEntity(eJson);
        }
    }

    return mScene;
}

void SceneManager::SaveCurrentScene(const SharedPtr<Scene>& scene)
{
    SaveScene(scene, activeScenePath.c_str());
}

void SceneManager::SaveScene(const SharedPtr<Scene>& scene, const std::string& savePath)
{
    // If no scene was currently loaded,
    // then designate this new path as the currently loaded scene
    // This is for cases where users create a new scene
    // and that new scene currently has no name associated with it
    if (!HasSceneLoaded())
    {
        activeScenePath.assign(savePath);
        activeSceneName = activeScenePath.substr(
                activeScenePath.find_last_of('/')+1
        );
    }

    json sceneJson;

    // Environment related
    sceneJson["Environment"]["skybox"] = scene->skybox->GetFacePaths();

    // Entity list
    scene->registry.each([&](auto entityID)
    {
        Entity entity = { entityID, scene.get() };
        if (!entity)
            return;

        sceneJson["Entities"].push_back(SerializeEntity(entity));
    });

    std::ofstream out(savePath);
    out << std::setw(2) << sceneJson << std::endl;
    out.close();

    LOG_INFO("Scene {0} has been saved at {1}", activeSceneName, savePath);
}

json SceneManager::SerializeEntity(Entity entity)
{
    json eJson;

    if (entity.HasComponent<IDComponent>())
    {
        eJson["IDComponent"] = entity.GetComponent<IDComponent>().ID;
    }

    if (entity.HasComponent<TagComponent>())
    {
        eJson["TagComponent"] = entity.GetComponent<TagComponent>().tag;
    }

    if (entity.HasComponent<TransformComponent>())
    {
        auto comp = entity.GetComponent<TransformComponent>();
        json cJson;
        glm::vec3 pos = comp.position;
        cJson["position"] = { pos.x, pos.y, pos.z };
        glm::vec3 euler = comp.euler;
        cJson["euler"] = { euler.x, euler.y, euler.z };
        glm::vec3 scale = comp.scale;
        cJson["scale"]    = { scale.x, scale.y, scale.z };

        eJson["TransformComponent"] = cJson;
    }

    if (entity.HasComponent<CameraComponent>())
    {
        auto cam = entity.GetComponent<CameraComponent>().camera;
        json cJson;
        cJson["targetDisplay"] = cam->targetDisplay;
        cJson["isPerspective"] = cam->isPerspective;
        cJson["vFov"] = cam->vFov;
        cJson["nearPlane"] = cam->nearPlane;
        cJson["farPlane"] = cam->farPlane;
        cJson["depth"] = cam->depth;
        cJson["x"] = cam->x;
        cJson["y"] = cam->y;
        cJson["width"] = cam->width;
        cJson["height"] = cam->height;
        cJson["size"] = cam->size;

        eJson["CameraComponent"] = cJson;
    }

    if (entity.HasComponent<MeshComponent>())
    {
        auto comp = entity.GetComponent<MeshComponent>().mesh;
        json cJson;
        cJson["filepath"] = comp->GetFilePath();

        eJson["MeshComponent"] = cJson;
    }

    if (entity.HasComponent<LightComponent>())
    {
        auto comp = entity.GetComponent<LightComponent>();
        json cJson;
        cJson["type"] = static_cast<uint32_t>(comp.type);
        cJson["color"] = { comp.color.r, comp.color.g, comp.color.b, comp.color.a };

        eJson["LightComponent"] = cJson;
    }

    if (entity.HasComponent<RigidbodyComponent>())
    {
        auto comp = entity.GetComponent<RigidbodyComponent>().rigidbody;
        json cJson;
        cJson["type"] = static_cast<uint32_t>(comp->type);
        cJson["mass"] = comp->mass;
        cJson["drag"] = comp->drag;
        cJson["angularDrag"] = comp->angularDrag;
        cJson["useGravity"] = comp->useGravity;
        cJson["freezePosition"] = { comp->freezePosition[0], comp->freezePosition[1], comp->freezePosition[2] };
        cJson["freezeRotation"] = { comp->freezeRotation[0], comp->freezeRotation[1], comp->freezeRotation[2] };

        eJson["RigidbodyComponent"] = cJson;
    }

    // TODO when separating shape from rb
    // try list of components
    if (entity.HasComponent<TriggerComponent>())
    {
    }

    if (entity.HasComponent<AudioComponent>())
    {
        auto comp = entity.GetComponent<AudioComponent>().audioSource;
        json cJson;
        cJson["clipPath"]    = comp->clipPath;
        cJson["mute"]        = comp->mute;
        cJson["playOnAwake"] = comp->playOnAwake;
        cJson["loop"]        = comp->loop;
        cJson["priority"]    = comp->priority;
        cJson["volume"]      = comp->volume;
        cJson["pitch"]       = comp->pitch;
        cJson["stereoPan"]   = comp->stereoPan;

        eJson["AudioComponent"] = cJson;
    }

    if (entity.HasComponent<ScriptComponent>())
    {
        auto comp = entity.GetComponent<ScriptComponent>();
        json cJson;
        cJson["filepath"] = comp.filepath;

        eJson["ScriptComponent"] = cJson;
    }

    if (entity.HasComponent<TextComponent>())
    {
    }

    if (entity.HasComponent<ParticleSystemComponent>())
    {
    }

    return eJson;
}

void SceneManager::DeserializeEntity(json eJson)
{
    auto tag = eJson["TagComponent"].get<std::string>();
    auto ID = eJson["IDComponent"].get<uint64_t>();
    Entity e = mScene->CreateEntity(tag, ID);

    if (eJson.contains("TransformComponent"))
    {
        auto cJson = eJson["TransformComponent"];
        auto& comp = e.GetComponent<TransformComponent>();
        auto pos = cJson["position"];
        comp.position = glm::vec3(pos[0], pos[1], pos[2]);

        auto euler = cJson["euler"];
        comp.euler = glm::vec3(euler[0], euler[1], euler[2]);

        auto scale = cJson["scale"];
        comp.scale = glm::vec3(scale[0], scale[1], scale[2]);
    }

    if (eJson.contains("CameraComponent"))
    {
        auto cJson = eJson["CameraComponent"];
        auto& comp = e.AddComponent<CameraComponent>();
        auto& cam = comp.camera;
        cam->targetDisplay = cJson["targetDisplay"];
        cam->isPerspective = cJson["isPerspective"];
        cam->vFov = cJson["vFov"];
        cam->nearPlane = cJson["nearPlane"];
        cam->farPlane = cJson["farPlane"];
        cam->depth = cJson["depth"];
        cam->x = cJson["x"];
        cam->y = cJson["y"];
        cam->width = cJson["width"];
        cam->height = cJson["height"];
        cam->size = cJson["size"];
    }

    if (eJson.contains("MeshComponent"))
    {
        auto cJson = eJson["MeshComponent"];
        auto& comp = e.AddComponent<MeshComponent>(cJson["filepath"].get<std::string>());
    }

    if (eJson.contains("LightComponent"))
    {
        auto cJson = eJson["LightComponent"];
        auto& comp = e.AddComponent<LightComponent>();
        comp.type = static_cast<LightComponent::LightType>(cJson["type"]);
        auto color = cJson["color"];
        comp.color = glm::vec4(color[0], color[1], color[2], color[3]);
    }

    if (eJson.contains("RigidbodyComponent"))
    {
        auto cJson = eJson["RigidbodyComponent"];
        auto& comp = e.AddComponent<RigidbodyComponent>();
        auto rb = comp.rigidbody;
        rb->type = static_cast<Rigidbody::BodyType>(cJson["type"]);
        rb->mass = cJson["mass"];
        rb->drag = cJson["drag"];
        rb->angularDrag = cJson["angularDrag"];
        rb->useGravity = cJson["useGravity"];
        auto fp = cJson["freezePosition"];
        for (int i = 0; i < 3; ++i)
            rb->freezePosition[i] = fp[i];
        auto fr = cJson["freezeRotation"];
        for (int i = 0; i < 3; ++i)
            rb->freezeRotation[i] = fr[i];
    }

    if (eJson.contains("TriggerComponent"))
    {
    }

    if (eJson.contains("AudioComponent"))
    {
        json cJson = eJson["AudioComponent"];
        auto& comp = e.AddComponent<AudioComponent>();
        auto audio = comp.audioSource;
        audio->clipPath = cJson["clipPath"];
        audio->mute = cJson["mute"];
        audio->playOnAwake = cJson["playOnAwake"];
        audio->loop = cJson["loop"];
        audio->priority = cJson["priority"];
        audio->volume = cJson["volume"];
        audio->pitch = cJson["pitch"];
        audio->stereoPan = cJson["stereoPan"];
    }

    if (eJson.contains("ScriptComponent"))
    {
        json cJson = eJson["ScriptComponent"];
        auto& comp = e.AddComponent<ScriptComponent>();
        comp.filepath = cJson["filepath"];
    }

    if (eJson.contains("TextComponent"))
    {
    }

    if (eJson.contains("ParticleSystemComponent"))
    {
    }
}

void SceneManager::SerializeScene(const std::string& filepath)
{

}
