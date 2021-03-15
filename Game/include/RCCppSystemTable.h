#pragma once
#include "RuntimeObjectSystem/RuntimeObjectSystem.h"
#include "RuntimeObjectSystem/ObjectInterfacePerModule.h"
#include "RCCppEntry.h"

static SystemTable*& g_SystemTable = PerModuleInterface::g_pSystemTable;

//We want pure virtual interfaces here
//Don't neeed to link as much
//https://github.com/RuntimeCompiledCPlusPlus/RuntimeCompiledCPlusPlus/wiki/Using-the-SystemTable-to-pass-interfaces-to-Runtime-Compiled-Code
struct SystemTable
{
    IRuntimeObjectSystem* runtimeObjectSystem = nullptr;
    IRCCppEntry* RCCppEntry = nullptr;
    ICompilerLogger* pLogger = nullptr;
};
