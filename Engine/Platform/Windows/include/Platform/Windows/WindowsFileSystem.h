#ifdef _WIN32

#ifndef WINDOWS_FILE_SYSTEM_H
#define WINDOWS_FILE_SYSTEM_H

#include <string>

class WindowsFileSystem
{
public:
    static std::string OpenFileName(const char*);
    static std::string SaveFileName(const char*);
    static void FindFiles(const char*);
    static bool CopyAFile(const char*, const char*, bool);
    static bool MoveFiles(const char*, const char*);
    static bool DeleteFiles(const char*);
    static bool OpenInWindowsExplorer(const char*);
    static bool OpenFileWithDefaultProgram(const char*);
    static void WindowsFileSystem::RunFileDirectoryWatcher(const char*);
};

#endif // WINDOWS_FILE_SYSTEM_H

#endif // _WIN32
