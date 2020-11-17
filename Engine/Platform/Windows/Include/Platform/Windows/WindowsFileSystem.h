#ifdef _WIN32

#ifndef WINDOWS_FILE_SYSTEM_H
#define WINDOWS_FILE_SYSTEM_H

#include <string>

class WindowsFileSystem
{
public:
    static std::string OpenFile(const char*);
    static std::string SaveFile(const char*);
};

#endif // WINDOWS_FILE_SYSTEM_H

#endif // _WIN32
