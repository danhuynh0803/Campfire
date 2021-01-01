#pragma once

#include "Core/Application.h"
#include "Core/ResourceManager.h"
#include <RuntimeObjectSystem/RuntimeObjectSystem.h>

extern Application* CreateApplication();

int main(int argc, char * argv[])
{

    Application* app = CreateApplication();
    app->Run();

    return 0;
}
