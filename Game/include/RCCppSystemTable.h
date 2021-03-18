#pragma once
#ifndef RCCPP_SYSTEMABLE_H
#define RCCPP_SYSTEMABLE_H

#include "RuntimeObjectSystem/IObject.h" 
#include "RuntimeObjectSystem/ObjectInterfacePerModule.h"
#include "RuntimeObjectSystem/IRuntimeObjectSystem.h"
#include "RuntimeObjectSystem/IObjectFactorySystem.h"
#include "RCCppEntry.h"
#include "Scene/Scene.h"
#include "Scene/Entity.h"
#include "Core/Base.h"
#include "Renderer/SceneRenderer.h"

#include "TestY.h"
#include "IEntitySystem.h"

#include <vector>
//#include "Scripting/Script.h"
//#include "entt.hpp"

static SystemTable*& g_SystemTable = PerModuleInterface::g_pSystemTable;

//We want pure virtual interfaces here
//https://github.com/RuntimeCompiledCPlusPlus/RuntimeCompiledCPlusPlus/wiki/Using-the-SystemTable-to-pass-interfaces-to-Runtime-Compiled-Code
struct SystemTable
{
    IObjectFactorySystem* pObjectFactorySystem = nullptr;
    IRuntimeObjectSystem* runtimeObjectSystem = nullptr;
    IRCCppEntry* RCCppEntry = nullptr;
    IEntitySystem* pEntitySystem = nullptr;
    ICompilerLogger* pLogger = nullptr;
    SharedPtr<Scene> activeScene = nullptr;
    SharedPtr<SceneRenderer> sceneRenderer = nullptr;
    std::vector<ITest*> tests;
};
#endif // !RCCPP_SYSTEMABLE.H