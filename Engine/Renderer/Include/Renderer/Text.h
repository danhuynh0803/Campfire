#pragma once

#include <string>
#include <map>

#include "Core/Base.h"
#include "Core/Log.h"
#include "Core/ResourceManager.h"
#include <glm/glm.hpp>
#include "Renderer/Texture.h"

//#include <ft2build.h>
//#include FT_FREETYPE_H

class Camera;
struct TransformComponent;

struct Character
{
    //SharedPtr<Texture2D> texture;
    uint32_t textureID;
    glm::ivec2 size;
    glm::ivec2 bearing;
    uint32_t advance;
};

class Font
{
public:
    virtual ~Font() = default;
    virtual std::string GetPath() const = 0;
    static SharedPtr<Font> Create(const std::string& fontPath, uint32_t fontSize = 12);
    std::map<char, Character> const GetCharacterMap() { return characters; }

protected:
    std::map<char, Character> characters;

private:
    static std::map<std::string, SharedPtr<Font>> fontCache;
};

class Text
{
public:
    virtual ~Text() = default;
    virtual void Draw(const TransformComponent& transform, const Camera& camera) = 0;
    void SetFont(SharedPtr<Font> fontPtr) { font = fontPtr; }
    SharedPtr<Font> GetFont() const { return font; }

    static SharedPtr<Text> Create(const std::string& newText = "New Text");

public:
    bool isUI = true;
    glm::vec4 color = glm::vec4(0, 0, 0, 1);
    std::string text = "New Text";
    uint32_t fontSize = 48;

    // NDC positional values for UI
    glm::vec2 uiPos = glm::vec2(0.5f);

protected:
    SharedPtr<Font> font = Font::Create(ASSETS + "Fonts/arial.ttf");
};

// TODO setup batching for all text objects
class TextEngine
{
public:
    static void Init();
    static void Shutdown();
    static void Draw(const std::string& text);
};
