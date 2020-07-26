#include "Core/FileSystem.h"
#include "Platform/Windows/WindowsFileSystem.h"
#include "Platform/Linux/LinuxFileSystem.h"

std::string FileSystem::OpenFile(const char* filter)
{
    // TODO swap to using a PLATFORM enum maybe?
    #ifdef PLATFORM_WINDOWS
        return WindowsFileSystem::OpenFile(filter);
    #else
        return LinuxFileSystem::OpenFile(filter);

    #endif

}

std::string FileSystem::SaveFile()
{
    #ifdef PLATFORM_WINDOWS
        return WindowsFileSystem::SaveFile();
    #else
        return LinuxFileSystem::SaveFile();
    #endif

}
