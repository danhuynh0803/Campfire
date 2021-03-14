#pragma once
#include "RuntimeObjectSystem/RuntimeObjectSystem.h"
#include "RuntimeObjectSystem/ObjectInterfacePerModule.h"
#include "RCCppEntry.h"

static SystemTable*& g_SystemTable = PerModuleInterface::g_pSystemTable;

struct SystemTable
{
    IRuntimeObjectSystem* runtimeObjectSystem = nullptr;
    IRCCppEntry* RCCppEntry = nullptr;
    ICompilerLogger* pLogger = nullptr;
};
