#pragma once

#include "Core/Application.h"
#include "Core/ResourceManager.h"
#include "Scripting/ScriptEngine.h"

extern Application* CreateApplication();

int main(int argc, char * argv[])
{
    ScriptEngine::Init();

    //ScriptEngine::Read(ASSETS + "Scripts/test.lua");

    Application* app = CreateApplication();
    app->Run();

    return 0;
}
