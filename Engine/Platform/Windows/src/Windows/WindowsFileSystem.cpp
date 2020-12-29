#include "Platform/Windows/WindowsFileSystem.h"
#include <algorithm>
#include <regex>
#include <sstream>
#include <string>
#include <windows.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <Core/Application.h>
#include <Core/Log.h>

#define MAX_PATH_LENGTH 260

//TODO: Use wchart method for unicode characters

std::string WSTRToSTR(const std::wstring& wstr)
{
    std::string strTo;
    char* buffer = new char[wstr.length() + 1];
    buffer[wstr.size()] = '\0';
    int result = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, buffer, (int)wstr.length(), NULL, NULL);
    switch (result)
    {
        case ERROR_INSUFFICIENT_BUFFER:
            break;
        case ERROR_INVALID_FLAGS:
            break;
        case ERROR_INVALID_PARAMETER:
            break;
        case ERROR_NO_UNICODE_TRANSLATION:
            break;
    }
    strTo = buffer;
    delete[] buffer;
    if (result != 0)
    {
        return "Error";
    }
    return strTo;
}

std::wstring STRToWSTR(const std::string& string)
{
    std::wstring wstrTo;
    wchar_t* buffer = new wchar_t[string.length() + 1];
    buffer[string.size()] = '\0';
    int result = MultiByteToWideChar(CP_ACP, 0, string.c_str(), -1, buffer, (int)string.length());
    switch (result)
    {
        case ERROR_INSUFFICIENT_BUFFER:
            break;
        case ERROR_INVALID_FLAGS:
            break;
        case ERROR_INVALID_PARAMETER:
            break;
        case ERROR_NO_UNICODE_TRANSLATION:
            break;
    }
    wstrTo = buffer;
    delete[] buffer;
    if (result != 0)
    {
        return L"Error";
    }
    return wstrTo;
}

//For COM
//Campfire::CoGenerator<std::string> FindFileInfo(HANDLE hFind, WIN32_FIND_DATA pNextInfo)
//{
//    while (FindNextFileA(hFind, &pNextInfo))
//    {
//        if (pNextInfo.cFileName[0] == '.')
//            continue;
//        std::string str(pNextInfo.cFileName);
//        co_yield str;
//    }
//}

std::string WindowsFileSystem::OpenFileName(const char* filter)
{
    // TODO convert filter to filter and filetype

    HWND hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow());
    OPENFILENAME ofn;
    char szFilename[MAX_PATH_LENGTH] = "";
    ZeroMemory(&ofn, sizeof(ofn));//=memset(&ofn, 0, sizeof(ofn));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hwndOwner;
    ofn.lpstrFilter = filter;
    ofn.lpstrFile = szFilename;
    ofn.nMaxFile = MAX_PATH_LENGTH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
    ofn.lpstrDefExt = "";
    std::string fileNameStr;
    if (GetOpenFileNameA(&ofn))
    {
        fileNameStr = szFilename;
    }

    // Replace '\' from windows filesystems with '/', which works on all platforms
    std::replace(fileNameStr.begin(), fileNameStr.end(), '\\', '/');

    return fileNameStr;
}

#include <iostream>;
std::string WindowsFileSystem::SaveFileName(const char* filter)
{
    // TODO refactor so it can handle multiple extensions
    // Replace with regex search
    std::string filterString(filter);
    auto openPos = filterString.find("(");
    auto closePos = filterString.find(")");
    std::string insideParen = filterString.substr(openPos + 1, closePos - openPos - 1);
    // ignore the asterisk within the file type
    std::string filetype = insideParen.substr(1);

    std::string sFilter;
    sFilter.append(filter);
    sFilter.push_back('\0');
    sFilter.append(insideParen.c_str());

    HWND hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow());
    OPENFILENAME ofn;
    char szFilename[MAX_PATH_LENGTH] = "";
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.nMaxFile = MAX_PATH_LENGTH;
    ofn.hwndOwner = hwndOwner;
    ofn.lpstrFile = szFilename;
    ofn.lpstrFilter = sFilter.c_str();
    ofn.Flags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
    std::string fileNameStr;

    if (GetSaveFileNameA(&ofn))
    {
        fileNameStr = szFilename;
        // Check if extension was added in file name, if not, then append filetype
        if (fileNameStr.find(filetype) == std::string::npos)
        {
            fileNameStr += filetype.c_str();
        }
        std::replace(fileNameStr.begin(), fileNameStr.end(), '\\', '/');
    }

    return fileNameStr;
}

void WindowsFileSystem::FindFiles(const char* fileName)
{
    //"{path}\\*.h" for all header file in target disk parition root directory
    //run this in a thread
    HANDLE hFind;
    LPCTSTR lpFileName = fileName;
    WIN32_FIND_DATA pNextInfo;
    hFind = FindFirstFileA(lpFileName, &pNextInfo);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        std::string firstFindFile = pNextInfo.cFileName;
        //auto fileInfoIterator = FindFileInfo(hFind, pNextInfo);
        while (FindNextFileA(hFind, &pNextInfo))
        {
            std::string nextFindFile(pNextInfo.cFileName);
        }
    }
    FindClose(hFind);
}

bool WindowsFileSystem::CopyAFile(const char* source, const char* target, bool checkIfAlreadyExist)
{
    if (!CopyFileA(source,target, checkIfAlreadyExist))
    {
        switch (GetLastError())
        {
            case ERROR_ACCESS_DENIED:
                LOG_ERROR("The destination file already exists. Fail to copy {0}", source);
                break;
            default:
                LOG_ERROR("Something went wrong :(");
                break;
        }
        return false;
    }
    return true;
}

bool WindowsFileSystem::MoveFiles(const char* source, const char* target)
{
    if (!MoveFileA(source, target))
    {
        switch (GetLastError())
        {
            case ERROR_ACCESS_DENIED:
                LOG_ERROR("Access denied. Fail to move {0}", source);
                break;
            default:
                LOG_ERROR("Something went wrong :(");
                break;
        }
        return false;
    }
    return true;
}

bool WindowsFileSystem::DeleteFiles(const char* fileName)
{
    if (!DeleteFileA(fileName))
    {
        switch (GetLastError())
        {
            case ERROR_FILE_NOT_FOUND:
                LOG_ERROR("{0} not found", fileName);
                break;
            case ERROR_ACCESS_DENIED:
                LOG_ERROR("Access denied. Fail to delete {0}", fileName);
                break;
            default:
                LOG_ERROR("Something went wrong :(");
                break;
        }
        return false;
    }
    return true;
}
bool WindowsFileSystem::OpenFileWithDefaultProgram(const char* filePath)
{
    const int res = (int)ShellExecuteA(NULL, "open", filePath, NULL, NULL, SW_SHOWDEFAULT);
    if (res <= 32)
    {
        switch (res)
        {
            case ERROR_PATH_NOT_FOUND:
                LOG_ERROR("Path not found. Fail to open {0}", filePath);
                break;
            case ERROR_ACCESS_DENIED:
                LOG_ERROR("Access is denied.");
                break;
            case ERROR_GEN_FAILURE:
                LOG_ERROR("Something went wrong. Try manually set the default program first.");
                break;
            default:
                LOG_ERROR("Something went wrong :(");
                break;
        }
        return 0;
    }
    return 1;
}

bool WindowsFileSystem::EditFileWithDefaultProgram(const char* filePath)
{
    const int res = (int)ShellExecuteA(NULL, "edit", filePath, NULL, NULL, SW_SHOWNORMAL);
    if (res <= 32)
    {
        switch (res)
        {
            case ERROR_PATH_NOT_FOUND:
                LOG_ERROR("Path not found. Fail to open {0}", filePath);
                break;
            case ERROR_ACCESS_DENIED:
                LOG_ERROR("Access is denied.");
                break;
            case ERROR_GEN_FAILURE:
                LOG_ERROR("Error code 32. Not sure what this mean yet :(");
                break;
            default:
                LOG_ERROR("Something went wrong :(");
                break;
        }
        return 0;
    }
    return 1;
}

bool WindowsFileSystem::OpenInWindowsExplorer(const char* fileDirectory)
{
    const int res = (int)ShellExecuteA(NULL, "explore", fileDirectory, NULL, NULL, SW_SHOWNORMAL);
    if (res <= 32)
    {
        switch (res)
        {
            case ERROR_PATH_NOT_FOUND:
                LOG_ERROR("Path not found. Fail to open directory {0}", fileDirectory);
                break;
            default:
                LOG_ERROR("Something went wrong :(");
                break;
        }
        return 0;
    }
    return 1;
}

void WindowsFileSystem::RunFileDirectoryWatcher(const char* watchDirectory)
{
    HANDLE hDirectory = CreateFile(watchDirectory,
        GENERIC_READ | GENERIC_WRITE | FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
        NULL);
    if (hDirectory == INVALID_HANDLE_VALUE)
    {
        DWORD dwErr = GetLastError();
        return;
    }

    char notify[1024];
    ZeroMemory(notify, 0, sizeof(notify));
    FILE_NOTIFY_INFORMATION* pNotification = (FILE_NOTIFY_INFORMATION*)notify;
    DWORD bytesReturned = 0;

    while (TRUE)
    {
        ZeroMemory(pNotification, sizeof(notify));

        auto watchState = ReadDirectoryChangesW(hDirectory,
            &notify,
            sizeof(notify),
            TRUE,   //Watch subdirectory
            FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE,
            (LPDWORD)&bytesReturned,
            NULL,
            NULL);

        if (GetLastError() == ERROR_INVALID_FUNCTION)
        {
            break;
        }
        else if (watchState == FALSE)
        {
            DWORD dwErr = GetLastError();
            break;
        }
        else if (GetLastError() == ERROR_NOTIFY_ENUM_DIR)
        {
            continue;
        }
        else
        {
            std::wstring fileName(pNotification->FileName, pNotification->FileNameLength / sizeof(wchar_t));
            //std::string fullFileName = watchDirectory + "/" + WSTRToStr(fileName);
            switch (pNotification->Action)
            {
                case FILE_ACTION_ADDED:
                    break;
                case FILE_ACTION_REMOVED:
                    break;
                case FILE_ACTION_MODIFIED:
                    break;
                case FILE_ACTION_RENAMED_OLD_NAME:
                    break;
            }
            //unicode gg
            CORE_INFO("{0}", WSTRToSTR(fileName).c_str());
        }
    }
    CloseHandle(hDirectory);
}

bool WindowsFileSystem::OpenCampFireGithubRepo()
{
    ShellExecuteA(NULL, "open", "https://github.com/danhuynh0803/Campfire", NULL, NULL, SW_SHOWDEFAULT);
    return true;
}

bool WindowsFileSystem::OpenCampFireGithubRepoTroubleshoot()
{
    ShellExecuteA(NULL, "open", "https://github.com/danhuynh0803/Campfire/issues", NULL, NULL, SW_SHOWDEFAULT);
    return true;
}

bool WindowsFileSystem::OpenCampFireGithubRepoWiki()
{
    ShellExecuteA(NULL, "open", "https://github.com/danhuynh0803/Campfire/wiki", NULL, NULL, SW_SHOWDEFAULT);
    return true;
}

bool WindowsFileSystem::OpenEmail(const char* email)
{
    ShellExecuteA(0, "open", email, 0, 0, 1);
    return true;
}