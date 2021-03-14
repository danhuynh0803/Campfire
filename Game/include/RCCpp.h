#pragma once

#include "RCCppEntry.h"
#include "RCCppSystemTable.h"
#include "StdioLogSystem.h"

enum PROJECTIDS
{
    PROJECTID_DEFAULT = 0,
    PROJECTID_MODULE_RCC
};

struct RCCpp
{
    static bool Init();
    static void Update(float dt);
    static void Shutdown();
    static SystemTable systemTable;
    static StdioLogSystem g_Logger;
};
