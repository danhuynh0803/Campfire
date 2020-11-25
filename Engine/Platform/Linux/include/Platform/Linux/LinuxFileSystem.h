#pragma once

#include <string>

class LinuxFileSystem
{
public:
    static std::string OpenFile(const char* filter);
    static std::string SaveFile(const char* filter);
    static bool MoveFiles(const char*, const char*);
    static bool DeleteFiles(const char*);
};
