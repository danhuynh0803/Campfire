#pragma once

#include "Core/Log.h"
#include <string>

struct Font
{
    Font(std::string path);
    std::string fontPath = "fonts/arial.ttf";
};

struct Text
{
    Font font;
    std::string text;
    int fontSize = 12;
};