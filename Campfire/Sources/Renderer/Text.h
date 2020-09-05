#pragma once

#include <string>
#include <map>

#include "Core/Base.h"
#include "Core/Log.h"
#include <glm/glm.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

struct Character
{
    glm::ivec2 size;
    glm::ivec2 bearing;
    uint32_t advance;
    uint32_t renderID; // texture info
};

class Font
{
public:
    virtual ~Font() = default;
    virtual std::string GetPath() const = 0;

protected:
    std::map<char, Character> characters;
};

class Text
{
public:
    std::string text;
    void SetFont(SharedPtr<Font> fontPtr) { font = fontPtr; }
    virtual void Draw() = 0;

protected:
    SharedPtr<Font> font = nullptr;
    int fontSize = 12;
};

class TextEngine
{
public:
    static void Init();
    static void Shutdown();
    static void Draw(const std::string& text);
    static SharedPtr<Font> CreateFont(const std::string& fontPath);

private:
    static FT_Library ft;
    static std::map<std::string, SharedPtr<Font>> TextEngine::fontCache;
};
