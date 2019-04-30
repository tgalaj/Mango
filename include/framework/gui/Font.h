#pragma once
#include <string>
#include "glad/glad.h"
#include <glm/vec2.hpp>
#include <map>

namespace Vertex
{
    class Font
    {
    public:
        struct Character
        {
            GLuint m_texture_id;
            glm::ivec2 m_size;
            glm::ivec2 m_bearing;
            GLuint m_advance;
        };

        Font() {}
        ~Font();

        void genFont(const std::string & filepathname, GLuint font_height);
        
        Character getCharacter(const char c) { return m_characters[c]; }

    private:
        std::map<GLchar, Character> m_characters;
    };
}
