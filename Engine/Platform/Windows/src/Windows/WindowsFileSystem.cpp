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

wchar_t* CharToWChar(const char* text)
{
    size_t size = strlen(text) + 1;
    wchar_t* wa = new wchar_t[size];
    mbstowcs(wa, text, size);
    return wa;
}

std::string WSTRToStr(const std::wstring& wstr)
{
    std::string strTo;
    char* szTo = new char[wstr.length() + 1];
    szTo[wstr.size()] = '\0';
    WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, szTo, (int)wstr.length(), NULL, NULL);
    strTo = szTo;
    delete[] szTo;
    return strTo;
}

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


std::string WindowsFileSystem::OpenFile(const char* filter)
{
    // TODO convert filter to filter and filetype

    HWND hwndOwner = glfwGetWin32Window((GLFWwindow*)Application::Get().GetWindow().GetNativeWindow());
    OPENFILENAME ofn;
    char szFilename[MAX_PATH] = "";
    ZeroMemory(&ofn, sizeof(ofn));//=memset(&ofn, 0, sizeof(ofn));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hwndOwner;
    ofn.lpstrFilter = filter;
    ofn.lpstrFile = szFilename;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
    ofn.lpstrDefExt = "";
    std::string fileNameStr;
    if (GetOpenFileName(&ofn))
    {
        fileNameStr = szFilename;
    }

    // Replace '\' from windows filesystems with '/', which works on all platforms
    std::replace(fileNameStr.begin(), fileNameStr.end(), '\\', '/');

    return fileNameStr;
}

#include <iostream>;
std::string WindowsFileSystem::SaveFile(const char* filter)
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
    char szFilename[MAX_PATH] = "";
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.nMaxFile = MAX_PATH;
    ofn.hwndOwner = hwndOwner;
    ofn.lpstrFile = szFilename;
    ofn.lpstrFilter = sFilter.c_str();
    ofn.Flags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
    std::string fileNameStr;

    if (GetSaveFileName(&ofn))
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
    //"{path}\\*.h" for all header file in C root directory
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

bool WindowsFileSystem::DeleteAFile(const char* fileName)
{
    if (!DeleteFileA(fileName))
    {
        switch (GetLastError())
        {
            case ERROR_FILE_NOT_FOUND:
                LOG_ERROR("{0} not found", fileName);
                break;
            case ERROR_ACCESS_DENIED:
                LOG_ERROR("{0} access denied", fileName);
                break;
            default:
                LOG_ERROR("Something went wrong :(");
                break;
        }
        return false;
    }
    return true;
}
