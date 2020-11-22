#include "OpenGL/OpenGLText.h"
#include "Scene/Component.h"
#include "Renderer/Renderer2D.h"
#include <freetype/freetype.h>

// ===================================
// Font
// ===================================
OpenGLFont::OpenGLFont(const std::string& fontPath, uint32_t fontSize)
    : path(fontPath)
{
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        CORE_ERROR("OpenGLFont::Unable to init library");
        return;
    }

    FT_Face face;
    // Check that font is valid
    if (FT_New_Face(ft, fontPath.c_str(), 0, &face))
    {
        CORE_ERROR("OpenGLFont::Failed to load font: {0}", fontPath);
        return;
    }

    // Set font's width and height parameters.
    // Setting the width to 0, allows face to dynamically calculate
    // width based on the given height
    FT_Set_Pixel_Sizes(face, 0, fontSize);

    // disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (unsigned char c = 0; c < 128; ++c)
    {
        // Load character glype
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            CORE_ERROR("OpenGLFont::Failed to load glype {0}", c);
            continue;
        }

        // Generate texture for glyph
        uint32_t textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );

        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        //SharedPtr<Texture2D> texture = Texture2D::Create(face->glyph->bitmap.width, face->glyph->bitmap.rows);
        //texture->SetData(textureID);
        // store character for later use
        Character character = {
            //texture,
            textureID,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<uint32_t>(face->glyph->advance.x)
        };

        characters.emplace(c, character);
    }

    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

// ===================================
// Text
// ===================================
OpenGLText::OpenGLText(const std::string& newText)
{
    text = newText;

    vertexArray = VertexArray::Create();

    vertexArray->Bind();

    vertexBuffer = VertexBuffer::Create(sizeof(float) * 6 * 5);
    vertexBuffer->Bind();
    BufferLayout layout =
    {
        { ShaderDataType::FLOAT3, "aPos" },
        { ShaderDataType::FLOAT2, "aUV" },
    };
    vertexBuffer->SetLayout(layout);

    vertexArray->AddVertexBuffer(vertexBuffer);

    vertexBuffer->Unbind();
    vertexArray->Unbind();

    shader = ShaderManager::Get("text");
    shader->Bind();
    shader->SetUniformBlock("Camera", 0);
}

void OpenGLText::Draw(const TransformComponent& transform, const Camera& camera)
{
    shader->Bind();
    shader->SetFloat4("color", color);
    shader->SetBool("isUI", isUI);
    glActiveTexture(GL_TEXTURE0);

    glm::vec3 pos = isUI ? glm::vec3(uiPos.x * camera.width, uiPos.y * camera.height, 0.0f) : transform.position;
    //glm::vec3 euler = glm::vec3(0.0f);
    glm::vec3 scale = transform.scale;

    vertexArray->Bind();

    auto characters = font->GetCharacterMap();
    for (auto c = text.begin(); c != text.end(); c++)
    {
        //glm::mat4 model = glm::mat4(1.0f);

        Character ch = characters[*c];

        float xpos = pos.x + ch.bearing.x * scale.x;
        float ypos = pos.y - (ch.size.y - ch.bearing.y) * scale.y;

        float w = ch.size.x * scale.x;
        float h = ch.size.y * scale.y;

        float vertices[6][5] = {
            { xpos,     ypos + h, pos.z,    0.0f, 0.0f },
            { xpos,     ypos,     pos.z,    0.0f, 1.0f },
            { xpos + w, ypos,     pos.z,    1.0f, 1.0f },

            { xpos,     ypos + h, pos.z,    0.0f, 0.0f },
            { xpos + w, ypos,     pos.z,    1.0f, 1.0f },
            { xpos + w, ypos + h, pos.z,    1.0f, 0.0f },
        };

        //model = glm::translate(model, glm::vec3(xpos, ypos, 0.0f));
        //model = glm::scale(model, glm::vec3(w, h, 1.0f));

        glBindTexture(GL_TEXTURE_2D, ch.textureID);

        vertexBuffer->Bind();
        vertexBuffer->SetData(vertices, sizeof(vertices));

        glDrawArrays(GL_TRIANGLES, 0, 6);

        // advance cursors for next glypth (advance = number of 1/64 pixels)
        // bitshift by 6 to get value in pixels (2^6 = 64)
        // so we get num of pixels by dividing 1/64 by 64
        pos.x += (ch.advance >> 6) * scale.x;
    }

    vertexArray->Unbind();
    glBindTexture(GL_TEXTURE_2D, 0);
}
