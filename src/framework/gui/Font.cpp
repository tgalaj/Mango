#include "framework/gui/Font.h"

#include <iostream>
#include <algorithm>
#include "helpers/Assertions.h"

Vertex::Font::~Font()
{
    if(m_texture_id != 0)
    {
        glDeleteTextures(1, &m_texture_id);
        m_texture_id = 0;
    }
}

void Vertex::Font::bindFontTexture(GLuint texture_unit)
{
    glBindTextureUnit(texture_unit, m_texture_id);
}

void Vertex::Font::genFont(const std::string& filepathname, GLuint font_height)
{
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return;
    }

    FT_Face face;
    if (FT_New_Face(ft, filepathname.c_str(), 0, &face))
    {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return;
    }

    FT_Set_Pixel_Sizes(face, 0, font_height);

    m_font_atlas_dimensions = calcFontAtlasDimensions(face);

    /* Allocate memory for texture atlas for this font */
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

    glGenTextures(1, &m_texture_id);
    glBindTexture(GL_TEXTURE_2D, m_texture_id);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RED,
                 m_font_atlas_dimensions.x,
                 m_font_atlas_dimensions.y,
                 0,
                 GL_RED,
                 GL_UNSIGNED_BYTE,
                 nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    genFontAtlasData(face, m_font_atlas_dimensions.x);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

glm::uvec2 Vertex::Font::calcFontAtlasDimensions(FT_Face face)
{
    glm::uvec2 dimensions(0);
    const FT_GlyphSlot glyph = face->glyph;

    unsigned roww = 0;
    unsigned rowh = 0;

    for(unsigned i = 32; i < 128; ++i)
    {
        if(FT_Load_Char(face, i, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }

        if(roww + glyph->bitmap.width + 1 >= VE_FONT_MAX_ATLAS_WIDTH)
        {
            dimensions.x = std::max(dimensions.x, roww);
            dimensions.y += rowh;
            roww = 0;
            rowh = 0;
        }

        roww += glyph->bitmap.width + 1;
        rowh = std::max(rowh, glyph->bitmap.rows);
    }

     dimensions.x = std::max(dimensions.y, roww);
     dimensions.y += rowh;

    return dimensions;
}

void Vertex::Font::genFontAtlasData(FT_Face face, GLuint font_atlas_width)
{
    VERTEX_ASSERT(font_atlas_width > 0);

    const FT_GlyphSlot glyph = face->glyph;
    unsigned offset_x = 0;
    unsigned offset_y = 0;
    unsigned rowh = 0;

    for(unsigned i = 32; i < 128; ++i)
    {
        if(FT_Load_Char(face, i, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }

        if(offset_x + glyph->bitmap.width + 1 >= VE_FONT_MAX_ATLAS_WIDTH)
        {
            offset_y += rowh;
            rowh = 0;
            offset_x = 0;
        }

        glTexSubImage2D(GL_TEXTURE_2D, 0, offset_x, offset_y, glyph->bitmap.width, glyph->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, glyph->bitmap.buffer);

        /* Cache metrics for each glyph */
        Character character = {
            glm::vec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::vec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            glm::vec2(face->glyph->advance.x >> 6, face->glyph->advance.y >> 6),
            glm::vec2(offset_x / float(m_font_atlas_dimensions.x), offset_y / float(m_font_atlas_dimensions.y))
        };
        m_characters.insert(std::pair<GLchar, Character>(i, character));

        rowh = std::max(rowh, glyph->bitmap.rows);
        offset_x += glyph->bitmap.width + 1;
    }
}
