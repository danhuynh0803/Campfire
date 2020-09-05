#include "Renderer/Text.h"

#include <ft2build.h>
#include FT_FREETYPE_H

Font::Font(std::string path)
{
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        LOG_ERROR("FreeType::Unable to init library");
    }

    FT_Face face;
    if (FT_New_Face(ft, fontPath.c_str(), 0, &face))
    {
        LOG_ERROR("FreeType::Failed to load {0}", fontPath);
    }
}