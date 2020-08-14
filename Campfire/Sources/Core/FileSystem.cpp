#include "Core/Base.h"
#include "Core/FileSystem.h"
#include "Platform/Windows/WindowsFileSystem.h"
#include "Platform/Linux/LinuxFileSystem.h"

std::string FileSystem::OpenFile(const char* filter)
{
    //#ifdef PLATFORM_WINDOWS
    //    return WindowsFileSystem::OpenFile(filter);
    //#elif PLATFORM_LINUX
    //    return LinuxFileSystem::OpenFile(filter);
    //#elif PLATFORM_MAC
    //#endif

    #ifdef PLATFORM_WINDOWS
        if(strcmp(filter,"cf"))
            return WindowsFileSystem::OpenFile("Campfire Files(*.cf)\0*.cf");
        else
            return WindowsFileSystem::OpenFile(filter);
    #else
        return LinuxFileSystem::OpenFile(filter);
    #endif

    return "ERROR";
}

std::string FileSystem::SaveFile(const char* filter)
{
    #ifdef PLATFORM_WINDOWS
    if (strcmp(filter, "*.cf"))
        return WindowsFileSystem::SaveFile("Campfire Files(*.cf)\0*.cf", ".cf");
    #else
        return LinuxFileSystem::SaveFile();
    #endif

    return "ERROR";
}
