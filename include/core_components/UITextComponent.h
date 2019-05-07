#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <string>
#include <utility>
#include <memory>
#include "framework/gui/Font.h"

namespace Vertex
{
    class UITextComponent
    {
    public:
        explicit UITextComponent(std::shared_ptr<Font> font, std::string text = "")
            : m_font(std::move(font)),
              m_text(std::move(text)),
              m_color(glm::vec4(1.0f)),
              m_position(glm::vec2(0.0)),
              m_scale(1.0f) {}

        std::shared_ptr<Font> m_font;
        std::string m_text;
        glm::vec4 m_color;
        glm::vec2 m_position;
        float m_scale;
    };
}
