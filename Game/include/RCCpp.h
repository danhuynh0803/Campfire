#pragma once
#ifndef RCCPP_H
#define RCCPP_H
#include "RuntimeObjectSystem/RuntimeProtector.h"
#include "RCCppSystemTable.h"
#include "StdioLogSystem.h"
#include "IGame.h"

//enum PROJECTIDS
//{
//    PROJECTID_DEFAULT = 0,
//    PROJECTID_MODULE_RCC
//};

struct RCCpp
{
    RCCpp(IGame* game);
    bool Init();
    //void Update(float dt);
    void Shutdown();
    SystemTable* systemTable;
    StdioLogSystem* g_Logger;
    ~RCCpp();
};
#endif // !RCCPP_H