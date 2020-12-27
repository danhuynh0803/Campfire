#include <iostream>
#include <stdio.h>
#include <string.h>

#include "Platform/Linux/LinuxFileSystem.h"
#include "Core/ResourceManager.h"

std::string LinuxFileSystem::OpenFile(const char* filter)
{
    std::string outputString;

    std::string cmd = "zenity --file-selection --filename=" + ASSETS + "/*";

    // FIXME filtering is broken from
    // switching to Windows filter format
    // File extension filtering
    //if (strcmp(filter, "") != 0)
    //{
    //    cmd += " --file-filter=";
    //    cmd += filter;
    //}
    FILE* outputStream;
    const int maxLength = 128;
    char outputLine[maxLength];
    // redirect standard output from cmd
    cmd += " 2>&1";

    outputStream = popen(cmd.c_str(), "r");
    if (outputStream)
    {
        while (!feof(outputStream))
        {
            if (fgets(outputLine, maxLength, outputStream) != NULL)
            {
                // Replace new line if present
                char* ptr = strchr(outputLine, '\n');
                if (ptr) { *ptr = '\0'; }

                outputString += outputLine;
            }
        }
        pclose(outputStream);
    }

    return outputString;
}

std::string LinuxFileSystem::SaveFile(const char* filter)
{
    std::string outputString;

    // FIXME relative paths dont seem to work when --save is set
    std::string cmd = "zenity --file-selection --save --confirm-overwrite --filename=${HOME}/repos/Campfire/Assets/";
    //std::string cmd = "zenity --file-selection --save --confirm-overwrite --filename=" + ASSETS;

    // FIXME filtering is broken from
    // switching to Windows filter format
    // File extension filtering
    //if (strcmp(filter, "") != 0)
    //{
    //    cmd += " --file-filter=";
    //    cmd += filter;
    //}
    const int maxLength = 128;
    char outputLine[maxLength];
    // redirect standard error from cmd
    cmd += " 2>&1";
    FILE* fp = popen(cmd.c_str(), "r");
    if (fp)
    {
        while (!feof(fp))
        {
            if (fgets(outputLine, maxLength, fp) != NULL)
            {
                // Replace new line if present
                char* ptr = strchr(outputLine, '\n');
                if (ptr) { *ptr = '\0'; }

                outputString += outputLine;
            }
        }
        pclose(fp);
    }

    return outputString;
}

bool LinuxFileSystem::MoveFiles(const char* source, const char* target)
{
    errno = 0;
    if (rename(source, target) < 0)
    {
        switch (errno)
        {
            case EIO:
                break;
            case EACCES:
                break;
            default:
                break;
        }
        return 0;
    }
    return 1;
}

bool LinuxFileSystem::DeleteFiles(const char* source)
{
    errno = 0;
    if (remove(source) < 0)
    {
        switch (errno)
        {
        case EIO:
            break;
        case EACCES:
            break;
        default:
            break;
        }
        return 0;
    }
    return 1;
}

bool LinuxFileSystem::OpenFileWithXDesktops(const char* filePath)
{
    int pid = fork();
    if (pid < 0)
    {
        return 0;
    }
    if (pid == 0) {
        execl("/usr/bin/xdg-open", "xdg-open", filePath, (char*)0);
        exit(1);
    }
    return 1;
}

bool LinuxFileSystem::OpenCampFireGithubRepo()
{
    int pid = fork();
    if (pid < 0)
    {
        return 0;
    }
    if (pid == 0) {
        execl("/usr/bin/xdg-open", "xdg-open", "https://github.com/danhuynh0803/Campfire", (char*)0);
        exit(1);
    }
    return 1;
}

bool LinuxFileSystem::OpenCampFireGithubRepoTroubleshoot()
{
    int pid = fork();
    if (pid < 0)
    {
        return 0;
    }
    if (pid == 0) {
        execl("/usr/bin/xdg-open", "xdg-open", "https://github.com/danhuynh0803/Campfire/issues", (char*)0);
        exit(1);
    }
    return 1;
}

bool LinuxFileSystem::OpenCampFireGithubRepoWiki()
{
    int pid = fork();
    if (pid < 0)
    {
        return 0;
    }
    if (pid == 0) {
        execl("/usr/bin/xdg-open", "xdg-open", "https://github.com/danhuynh0803/Campfire/wiki", (char*)0);
        exit(1);
    }
    return 1;
}
