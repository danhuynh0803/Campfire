#ifndef LINUX_FILE_SYSTEM_H
#define LINUX_FILE_SYSTEM_H

#include <string>

class LinuxFileSystem
{
public:
    static std::string OpenFile(const char* filter);
    static std::string SaveFile();
};


#endif // LINUX_FILE_SYSTEM_H
