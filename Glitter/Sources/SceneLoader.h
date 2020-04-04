#ifndef SCENE_LOADER_H
#define SCENE_LOADER_H

#include "ObjectManager.h"

class SceneLoader
{
public:
    void LoadScene(ObjectManager&, const char*);
    void SaveScene(ObjectManager&, const char*);
    void DeleteScene();
};

#endif // SCENE_LOADER_H
