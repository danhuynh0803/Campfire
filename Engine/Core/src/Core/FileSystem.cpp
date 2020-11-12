#include "Core/Base.h"
#include "Core/FileSystem.h"

#ifdef PLATFORM_WINDOWS
#include "Platform/Windows/WindowsFileSystem.h"
#else
#include "Platform/Linux/LinuxFileSystem.h"
#endif

std::string FileSystem::OpenFile(const char* filter)
{
    #ifdef PLATFORM_WINDOWS
        return WindowsFileSystem::OpenFile(filter);
    #else
        return LinuxFileSystem::OpenFile(filter);
    #endif

    return "ERROR";
}

std::string FileSystem::SaveFile(const char* filter)
{
    #ifdef PLATFORM_WINDOWS
        return WindowsFileSystem::SaveFile(filter);
    #else
        return LinuxFileSystem::SaveFile(filter);
    #endif

    return "ERROR";
}
