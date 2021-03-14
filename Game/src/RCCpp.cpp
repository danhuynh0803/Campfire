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
    systemTable.runtimeObjectSystem->AddIncludeDir(includeDir.c_str(),PROJECTID_DEFAULT);
    //systemTable.runtimeObjectSystem->AddIncludeDir(rccDir.c_str(), PROJECTID_MODULE_RCC);
    //FileSystemUtils::Path RCCIncludeDir = basePath.ParentPath()/"Vendor"/"RuntimeCompiledCPlusPlus"/"Aurora"/"RuntimeObjectSystem";
    //systemTable.runtimeObjectSystem->AddIncludeDir(RCCIncludeDir.c_str(), PROJECTID_MODULE_RCC);
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
