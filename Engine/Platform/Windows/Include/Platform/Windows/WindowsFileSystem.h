#ifdef _WIN32

#ifndef WINDOWS_FILE_SYSTEM_H
#define WINDOWS_FILE_SYSTEM_H

#include <string>

class WindowsFileSystem
{
public:
    static std::string OpenFile(const char*);
    static std::string SaveFile(const char*);
    static void FindFiles(const char*);
    static bool CopyAFile(const char*, const char*, bool);
    static bool MoveFiles(const char*, const char*);
    static bool DeleteAFile(const char*);
    static void OpenInExplorer(const char*);
    static void WindowsFileSystem::RunFileDirectoryWatcher(const char*);
};

#endif // WINDOWS_FILE_SYSTEM_H

#endif // _WIN32
