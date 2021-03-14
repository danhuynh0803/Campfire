#include "RuntimeObjectSystem/IObject.h"
#include "RuntimeObjectSystem/ObjectInterfacePerModule.h"
#include "RuntimeObjectSystem/IObjectFactorySystem.h"
#include "RCCppEntry.h"
#include "RCCppSystemTable.h"

//#include "RuntimeLinkLibrary.h"
//RUNTIME_COMPILER_LINKLIBRARY("d3d11.lib");

//#include "RuntimeObjectSystem/RuntimeSourceDependency.h"
//RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("imgui/imgui", ".cpp");
//RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("imgui/imgui_widgets", ".cpp");
//RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("imgui/imgui_draw", ".cpp");
//RUNTIME_COMPILER_SOURCEDEPENDENCY_FILE("imgui/imgui_demo", ".cpp");
#include <iostream>

enum InterfaceIDEntryEnum
{
    IID_IRCCPP_ENTRY = IID_ENDInterfaceID
};

struct RCCppEntry : IRCCppEntry, TInterface<IID_IRCCPP_ENTRY, IObject>
{
    RCCppEntry()
    {
        g_SystemTable->RCCppEntry = this;
        g_SystemTable->runtimeObjectSystem->GetObjectFactorySystem()->SetObjectConstructorHistorySize(10);
    }

    void Update(float dt) override
    {
        int i = 0;
        int j = 0;
        int x = 4;
        std::cout << "-";
        
    }
};

REGISTERSINGLETON(RCCppEntry, true);