#pragma once

#include <string>
#include <vector>
#include <filesystem>

class FileSystem
{
public:
    static std::string OpenFile(const char* filter = "");
    static bool OpenFileWithDefaultProgram(const char* filter = "");
    static bool EditFileWithDefaultProgram(const char* filePath);
    static bool OpenInDirectory(const char* filePath);
    static std::string SaveFile(const char* filter = "");
    static std::vector<std::filesystem::path> GetAllFiles(const char* root, const char* ext);
    static bool OpenCampFireGithubRepo();
    static bool OpenCampFireGithubRepoTroubleshoot();
    static bool OpenCampFireGithubRepoWiki();
    static bool OpenEmail(const char*);

private:
    static bool restrictToAssets;
};
