#include "RCCpp.h"

SystemTable RCCpp::systemTable;
StdioLogSystem RCCpp::g_Logger;

bool RCCpp::Init()
{
    systemTable.runtimeObjectSystem = new RuntimeObjectSystem;
    systemTable.pLogger = &g_Logger;

    if (!systemTable.runtimeObjectSystem->Initialise(&g_Logger, &systemTable))
    {
        delete systemTable.runtimeObjectSystem;
        systemTable.runtimeObjectSystem = nullptr;
        return false;
    }

    systemTable.runtimeObjectSystem->CleanObjectFiles();
    
    // ensure include directories are set - use location of this file as starting point
    FileSystemUtils::Path basePath = systemTable.runtimeObjectSystem->FindFile(__FILE__);
    FileSystemUtils::Path includeDir = basePath.ParentPath().ParentPath()/ "include";
    //FileSystemUtils::Path rccDir = basePath.ParentPath().ParentPath().ParentPath() / "vendor"/ "runtimeCompiledCPlusPlus" / "aurora";
    systemTable.runtimeObjectSystem->AddIncludeDir(includeDir.c_str());
    FileSystemUtils::Path RCCppIncludeDir = basePath.ParentPath().ParentPath().ParentPath()/"Vendor"/"RuntimeCompiledCPlusPlus"/"Aurora";
    systemTable.runtimeObjectSystem->AddIncludeDir(RCCppIncludeDir.c_str());
    return true;
}

void RCCpp::Shutdown()
{
    delete systemTable.runtimeObjectSystem;
}

void RCCpp::Update(float dt)
{
    //check status of any compile
    if (systemTable.runtimeObjectSystem->GetIsCompiledComplete())
    {
        // load module when compile complete
        systemTable.runtimeObjectSystem->LoadCompiledModule();
    }

    if (!systemTable.runtimeObjectSystem->GetIsCompiling())
    {
        systemTable.runtimeObjectSystem->GetFileChangeNotifier()->Update(dt);
    }
    //else
    //{
    //    ResetPowerSaveCountDown();
    //}
}
