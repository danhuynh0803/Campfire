#include "../../Vendor/RuntimeCompiledCPlusPlus/Aurora/RuntimeObjectSystem/IObject.h"
#include "../../Vendor/RuntimeCompiledCPlusPlus/Aurora/RuntimeObjectSystem/ObjectInterfacePerModule.h"
#include "../../Vendor/RuntimeCompiledCPlusPlus/Aurora/RuntimeObjectSystem/IObjectFactorySystem.h"
//#include "RuntimeObjectSystem/IObjectFactorySystem.h"
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
    }
};

REGISTERSINGLETON(RCCppEntry, true);