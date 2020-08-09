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
        return WindowsFileSystem::OpenFile(filter);
    #elif
        return LinuxFileSystem::OpenFile(filter);
    #endif

    return "ERROR";
}

std::string FileSystem::SaveFile()
{
    #ifdef PLATFORM_WINDOWS
        return WindowsFileSystem::SaveFile();
    #elif PLATFORM_LINUX
        return LinuxFileSystem::SaveFile();
    #elif PLATFORM_MAC
    #endif

    return "ERROR";
}
