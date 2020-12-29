#ifdef _WIN32

#ifndef WINDOWS_FILE_SYSTEM_H
#define WINDOWS_FILE_SYSTEM_H

#include <string>

namespace WindowsFileSystem
{
    std::string OpenFileName(const char*);
    std::string SaveFileName(const char*);
    void FindFiles(const char*);
    bool CopyAFile(const char*, const char*, bool);
    bool MoveFiles(const char*, const char*);
    bool DeleteFiles(const char*);
    bool OpenInWindowsExplorer(const char*);
    bool OpenFileWithDefaultProgram(const char*);
    bool EditFileWithDefaultProgram(const char*);
    void RunFileDirectoryWatcher(const char*);
    bool OpenCampFireGithubRepo();
    bool OpenCampFireGithubRepoTroubleshoot();
    bool OpenCampFireGithubRepoWiki();
    bool OpenEmail(const char*);
};

#endif // WINDOWS_FILE_SYSTEM_H

#endif // _WIN32
