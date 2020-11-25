#include "Platform/Linux/LinuxFileSystem.h"
#include <iostream>
#include <stdio.h>
#include <string.h>

std::string LinuxFileSystem::OpenFile(const char* filter)
{
    std::string outputString;

    std::string cmd = "zenity --file-selection";

    // File extension filtering
    if (strcmp(filter, "") != 0)
    {
        cmd += " --file-filter=";
        cmd += filter;
    }
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
    return "";
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
