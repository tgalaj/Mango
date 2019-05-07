#pragma once
#include "glad/glad.h"

#include <string>
#include <glm/vec2.hpp>
#include <map>

#include <ft2build.h>
#include FT_FREETYPE_H

#define VE_FONT_MAX_ATLAS_WIDTH 1024

namespace Vertex
{
    class Font
    {
    public:
        struct Character
        {
            glm::vec2 m_size;
            glm::vec2 m_bearing;
            glm::vec2 m_advance;
            glm::vec2 m_glyph_texture_offset;
        };

        Font() {}
        ~Font();

        void bindFontTexture(GLuint texture_unit = 0);
        void genFont(const std::string & filepathname, GLuint font_height);
        Character getCharacter(const char c) { return m_characters[c]; }
        glm::ivec2 getFontAtlasDimensions() const { return m_font_atlas_dimensions; }

    private:
        glm::uvec2 calcFontAtlasDimensions(FT_Face face);
        void genFontAtlasData(FT_Face face, GLuint font_atlas_width);

        std::map<GLchar, Character> m_characters;
        glm::ivec2 m_font_atlas_dimensions;
        GLuint m_texture_id;
    };
}
