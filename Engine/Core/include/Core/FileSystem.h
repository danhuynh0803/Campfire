#pragma once

#include <string>
#include <vector>
#include <filesystem>

class FileSystem
{
public:
    static std::string OpenFile(const char* filter = "");
    static std::string SaveFile(const char* filter = "");
    static std::vector<std::filesystem::path> GetAllFiles(const char* root, const char* ext);

private:
    static bool restrictToAssets;
};
