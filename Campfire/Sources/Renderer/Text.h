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
    uint32_t renderID; // texture info
    glm::ivec2 size;
    glm::ivec2 bearing;
    uint32_t advance;
};

class Font
{
public:
    virtual ~Font() = default;
    virtual std::string GetPath() const = 0;
    static SharedPtr<Font> Create(const std::string& fontPath);

protected:
    std::map<char, Character> characters;

private:
    static std::map<std::string, SharedPtr<Font>> fontCache;
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
};
