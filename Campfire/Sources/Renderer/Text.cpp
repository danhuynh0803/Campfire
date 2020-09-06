#include "Renderer/Text.h"
#include "Renderer/RendererAPI.h"
#include "Platform/OpenGL/OpenGLText.h"

std::map<std::string, SharedPtr<Font>> Font::fontCache;

SharedPtr<Font> Font::Create(const std::string& fontPath)
{
    // Check if font has already been loaded up prior
    if (fontCache.find(fontPath) != fontCache.end())
    {
        return fontCache[fontPath];
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

    fontCache.emplace(fontPath, newFont);

    return newFont;
}

SharedPtr<Text> Text::Create(const std::string& newText)
{
    switch (RendererAPI::GetAPI())
    {
        case RendererAPI::API::None:
            return nullptr;
        case RendererAPI::API::OpenGL:
            return CreateSharedPtr<OpenGLText>(newText);
    }
}


void TextEngine::Init()
{
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

// =======================================
// Text Engine
// =======================================
