#ifndef WINDOWS_FILE_SYSTEM_H
#define WINDOWS_FILE_SYSTEM_H

#ifdef _WIN32
#  include <windows.h>
#endif

#include <string>

class WindowsFileSystem
{
public:
    static std::string OpenFile(const char* filter);
    static std::string SaveFile();
};

#endif // WINDOWS_FILE_SYSTEM_H
