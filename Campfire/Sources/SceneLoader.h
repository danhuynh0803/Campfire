#ifndef SCENE_LOADER_H
#define SCENE_LOADER_H

#include "ObjectManager.h"

class SceneLoader
{
public:
    // TODO swap to using pointer instead of reference here
    // since shared data uses references
    // Either switch to references or pointers
    void LoadNewScene(ObjectManager&);
    void LoadScene(ObjectManager&, const char*);
    void SaveScene(ObjectManager&, const char*);
    void SaveCurrentScene(ObjectManager&);

    void DeleteScene();

    bool HasSceneLoaded()
    {
        return !(currentScenePath.empty());
    }

private:
    std::string currentScenePath;
    std::string currentSceneFileName;
};

#endif // SCENE_LOADER_H
