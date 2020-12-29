#pragma once

#include <string>

namespace LinuxFileSystem
{
    std::string OpenFile(const char*);
    std::string SaveFile(const char*);
    bool OpenFileWithDefaultProgram(const char*);
    bool MoveFiles(const char*, const char*);
    bool DeleteFiles(const char*);
    bool OpenFileWithXDesktops(const char*);
    bool OpenCampFireGithubRepo();
    bool OpenCampFireGithubRepoTroubleshoot();
    bool OpenCampFireGithubRepoWiki();
};
