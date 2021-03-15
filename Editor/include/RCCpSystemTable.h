#pragma once
#include "RuntimeObjectSystem/RuntimeObjectSystem.h"
#include "RuntimeObjectSystem/ObjectInterfacePerModule.h"

static SystemTable*& g_SystemTable = PerModuleInterface::g_pSystemTable;

struct SystemTable
{
    IRuntimeObjectSystem* runtimeObjectSystem = nullptr;
    ICompilerLogger* pLogger = nullptr;
};
