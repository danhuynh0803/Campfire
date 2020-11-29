#include "Core/Base.h"
#include "Core/FileSystem.h"
#include "Core/ResourceManager.h"

#ifdef PLATFORM_WINDOWS
#include "Platform/Windows/WindowsFileSystem.h"
#else
#include "Platform/Linux/LinuxFileSystem.h"
#endif

// To force loading of just assets within the "Assets" dir.
// This sets filepath to be relative to the "Assets" dir
bool FileSystem::restrictToAssets = true;

std::string FileSystem::OpenFile(const char* filter)
{
    std::string path;
    #ifdef PLATFORM_WINDOWS
        path = WindowsFileSystem::OpenFile(filter);
    #else
        path = LinuxFileSystem::OpenFile(filter);
    #endif

    if (restrictToAssets)
    {
        std::size_t found = path.rfind("Assets");
        if (found == std::string::npos)
        {
            LOG_ERROR("Please import {0} into Assets or file will be saved with the absolute path!", path);
        }
        else
        {
            // Update path to be relative to the "Assets" directory
            path = ASSETS + path.substr(found + 7);
        }
    }

    return path;
}

std::string FileSystem::SaveFile(const char* filter)
{
    #ifdef PLATFORM_WINDOWS
        return WindowsFileSystem::SaveFile(filter);
    #else
        return LinuxFileSystem::SaveFile(filter);
    #endif

    return "";
}

std::vector<std::filesystem::path> FileSystem::GetAllFiles(const char* root, const char* ext)
{
    std::vector<std::filesystem::path> matches;
    for (auto f : std::filesystem::recursive_directory_iterator(root))
    {
        if (f.path().extension() == ext) {
            matches.emplace_back(f.path());
        }
    }

    return matches;
}
