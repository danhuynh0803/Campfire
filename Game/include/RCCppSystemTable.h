#pragma once
#ifndef RCCPP_SYSTEMABLE_H
#define RCCPP_SYSTEMABLE_H

#include "Definition.inl"
#include "RuntimeObjectSystem/ObjectInterfacePerModule.h"
#include "Scene/Scene.h"
#include "Renderer/SceneRenderer.h"
#include "RCCppEntry.h"
#include "Scene/Entity.h"

//static SystemTable*& g_SystemTable = PerModuleInterface::g_pSystemTable;

//We want pure virtual interfaces here
//https://github.com/RuntimeCompiledCPlusPlus/RuntimeCompiledCPlusPlus/wiki/Using-the-SystemTable-to-pass-interfaces-to-Runtime-Compiled-Code
struct SystemTable
{
    IGame* pGame = nullptr;
    IObjectFactorySystem* pObjectFactorySystem = nullptr;
    IRuntimeObjectSystem* runtimeObjectSystem = nullptr;
    IEntitySystem* pEntitySystem = nullptr;
    IRCCppEntry* RCCppEntry = nullptr;
    
    std::shared_ptr<Scene> activeScene = nullptr;
    std::shared_ptr<SceneRenderer> sceneRenderer = nullptr;
};
#endif // !RCCPP_SYSTEMABLE.H