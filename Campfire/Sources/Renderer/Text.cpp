#include "Renderer/Text.h"
#include "Renderer/RendererAPI.h"
#include "Platform/OpenGL/OpenGLText.h"

// =======================================
// Text Engine
// =======================================
FT_Library TextEngine::ft;

SharedPtr<Font> TextEngine::CreateFont(const std::string& fontPath)
{
    FT_Face face;
    // Check that font is valid
    if (FT_New_Face(ft, fontPath.c_str(), 0, &face))
    {
        LOG_ERROR("FreeType::Failed to load {0}", fontPath);
        return nullptr;
    }

    // Check if font has already been loaded up prior
    if (fontCache.find(fontPath) != TextEngine::fontCache.end())
    {
        return TextEngine::fontCache[fontPath];
    }

    // Font does not exist in cache, so initialize this new font
    SharedPtr<Font> newFont = nullptr;
    switch (RendererAPI::GetAPI())
    {
        case RendererAPI::API::None:
            return nullptr;
        case RendererAPI::API::OpenGL:
            newFont = CreateSharedPtr<OpenGLFont>(fontPath);
    }

    TextEngine::fontCache.emplace(fontPath, newFont);

    return newFont;
}

std::map<std::string, SharedPtr<Font>> TextEngine::fontCache;

void TextEngine::Init()
{
    if (FT_Init_FreeType(&ft))
    {
        LOG_ERROR("FreeType::Unable to init library");
    }
}

void TextEngine::Shutdown()
{
}

/*
void TextEngine::Draw(Text text)
{
}
*/

void TextEngine::Draw(const std::string& text)
{
}

// =======================================
// Text
// =======================================
