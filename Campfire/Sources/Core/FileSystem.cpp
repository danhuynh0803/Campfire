#include "Core/FileSystem.h"
#include "Platform/Windows/WindowsFileSystem.h"

std::string FileSystem::OpenFile(const char* filter)
{
    // TODO swap to using a PLATFORM enum maybe?
    #ifdef PLATFORM_WINDOWS
        return WindowsFileSystem::OpenFile(filter);
    #elif PLATFORM_LINUX

    #endif

    return WindowsFileSystem::OpenFile(filter);
}

std::string FileSystem::SaveFile()
{
    #ifdef PLATFORM_WINDOWS
        return WindowsFileSystem::SaveFile();
    #elif PLATFORM_LINUX

    #endif

    return WindowsFileSystem::SaveFile();
}
