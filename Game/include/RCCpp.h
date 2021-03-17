#pragma once
#ifndef RCCPP_H
#define RCCPP_H
#include "RCCppSystemTable.h"
#include "StdioLogSystem.h"
#include "TestY.h"

//enum PROJECTIDS
//{
//    PROJECTID_DEFAULT = 0,
//    PROJECTID_MODULE_RCC
//};

struct RCCpp
{
    static bool Init();
    static void Update(float dt);
    static void Shutdown();
    static SystemTable systemTable;
    static StdioLogSystem g_Logger;
};
#endif // !RCCPP_H