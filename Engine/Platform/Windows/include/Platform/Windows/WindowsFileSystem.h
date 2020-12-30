#ifdef _WIN32

#ifndef WINDOWS_FILE_SYSTEM_H
#define WINDOWS_FILE_SYSTEM_H

#include <string>

namespace WindowsFileSystem
{
    std::string WSTRToSTR(const std::wstring& wstr);
    std::wstring STRToWSTR(const std::string& string);
    std::string OpenFileName(const char* filter);
    std::string SaveFileName(const char* filter);
    void FindFiles(const char* fileName);
    bool CopyAFile(const char* source, const char* target, bool checkIfAlreadyExist);
    bool MoveFiles(const char* source, const char* target);
    bool DeleteFiles(const char* fileName);
    bool OpenInWindowsExplorer(const char* filePath);
    bool OpenFileWithDefaultProgram(const char* filePath);
    bool EditFileWithDefaultProgram(const char* fileDirectory);
    void RunFileDirectoryWatcher(const char* watchDirectory);
    bool OpenCampFireGithubRepo();
    bool OpenCampFireGithubRepoTroubleshoot();
    bool OpenCampFireGithubRepoWiki();
    bool OpenEmail(const char*);
};

#endif // WINDOWS_FILE_SYSTEM_H

#endif // _WIN32
