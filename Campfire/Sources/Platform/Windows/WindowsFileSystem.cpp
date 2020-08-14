#include "Platform/Windows/WindowsFileSystem.h"
#include <algorithm>

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


std::string WindowsFileSystem::OpenFile(const char* filter)
{
    HWND hwndOwner = NULL;
    OPENFILENAME ofn;
    char szFilename[MAX_PATH] = "";
    ZeroMemory(&ofn, sizeof(ofn));
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

std::string WindowsFileSystem::SaveFile(const char* filter)
{
    HWND hwndOwner = NULL;
    OPENFILENAME ofn;
    char szFilename[MAX_PATH] = "";
    ofn.lStructSize = sizeof(ofn);
    ofn.nMaxFile = MAX_PATH;
    ofn.hwndOwner = hwndOwner;
    ofn.lpstrFile = szFilename;
    ofn.Flags = OFN_HIDEREADONLY |
        OFN_OVERWRITEPROMPT;
    std::string fileNameStr;

    if (GetSaveFileName(&ofn))
    {
        fileNameStr = szFilename;
        std::replace(fileNameStr.begin(), fileNameStr.end(), '\\', '/');
    }

    return fileNameStr;
}
