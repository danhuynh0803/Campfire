#ifndef WINDOWS_FILE_SYSTEM_H
#define WINDOWS_FILE_SYSTEM_H

#ifdef _WIN32
#  include <windows.h>
#endif

#include <string>
#include "CDialogEventHandler.h"

class WindowsFileSystem
{
public:
    static std::string OpenFile(const char* filter);
    static std::string SaveFile();
    static HRESULT hr;
};

#endif // WINDOWS_FILE_SYSTEM_H