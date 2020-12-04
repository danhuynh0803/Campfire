#pragma once

#include <string>

class LinuxFileSystem
{
public:
    static std::string OpenFile(const char*);
    static std::string SaveFile(const char*);
    static bool OpenFileWithDefaultProgram(const char*);
    static bool MoveFiles(const char*, const char*);
    static bool DeleteFiles(const char*);
};
