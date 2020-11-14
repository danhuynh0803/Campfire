#pragma once

#include <string>
#include <nlohmann/json.hpp>

#include "Core/Base.h"
#include "Scene.h"
#include "Entity.h"

using json = nlohmann::json;

class SceneManager
{
public:
    static SharedPtr<Scene> LoadNewScene();
    static SharedPtr<Scene> LoadScene(const std::string& loadPath);
    static void SaveScene(const SharedPtr<Scene>& scene, const std::string& savePath);
    static void SaveCurrentScene(const SharedPtr<Scene>& scene);
    static void DeleteScene();

    static std::string GetActiveScene() { return activeScenePath; }
    static std::string GetActiveSceneName() { return activeSceneName; }
    static bool HasSceneLoaded() { return !(activeScenePath.empty()); }

private:
    static json SerializeEntity(Entity entity);
    static void DeserializeEntity(json eJson);

    void SerializeScene(const std::string& filepath);
    void SerializeSceneRuntime(const std::string& filepath);
    bool DeserializeScene(const std::string& filepath);
    bool DeserializeSceneRuntime(const std::string& filepath);

private:
    Entity LoadEntity();
    void SaveEntity(const Entity& entity);
    static std::string activeScenePath;
    static std::string activeSceneName;
    static SharedPtr<Scene> mScene;

    static std::map<std::string, SharedPtr<Scene>> mCachedSceneMap;
};
