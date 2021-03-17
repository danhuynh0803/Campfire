#pragma once
#ifndef RCCPP_SYSTEMABLE_H
#define RCCPP_SYSTEMABLE_H

#include "RuntimeObjectSystem/IObject.h" 
#include "RuntimeObjectSystem/ObjectInterfacePerModule.h"
#include "RuntimeObjectSystem/IRuntimeObjectSystem.h"
#include "RCCppEntry.h"
#include <vector>
#include "Scene/Scene.h"
#include "Core/Base.h"
#include "Scene/Entity.h"
#include "Renderer/SceneRenderer.h"
#include "TestY.h"

//#include "Scripting/Script.h"
//#include "entt.hpp"

static SystemTable*& g_SystemTable = PerModuleInterface::g_pSystemTable;

//We want pure virtual interfaces here
//Don't neeed to link as much
//https://github.com/RuntimeCompiledCPlusPlus/RuntimeCompiledCPlusPlus/wiki/Using-the-SystemTable-to-pass-interfaces-to-Runtime-Compiled-Code
struct SystemTable
{
    IRuntimeObjectSystem* runtimeObjectSystem = nullptr;
    IRCCppEntry* RCCppEntry = nullptr;
    //ITest* test = nullptr;
    SharedPtr<Scene> activeScene = nullptr;
    SharedPtr<SceneRenderer> sceneRenderer = nullptr;
    std::vector<ITest*> tests;
    ICompilerLogger* pLogger = nullptr;
};
#endif // !RCCPP_SYSTEMABLE.H