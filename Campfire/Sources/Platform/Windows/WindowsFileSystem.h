#ifndef WINDOWS_FILE_SYSTEM_H
#define WINDOWS_FILE_SYSTEM_H

#include <string>

class WindowsFileSystem
{
public:
    static std::string OpenFile(const char* filter);
    static std::string SaveFile();
};

#endif // WINDOWS_FILE_SYSTEM_H
