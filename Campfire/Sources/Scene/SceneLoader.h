#ifndef SCENE_LOADER_H
#define SCENE_LOADER_H

#include <string>
#include "Core/Base.h"
#include "Scene/Scene.h"

class SceneLoader
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
    Entity LoadEntity();
    void SaveEntity(const Entity& entity);
    static std::string activeScenePath;
    static std::string activeSceneName;
};

#endif // SCENE_LOADER_H
