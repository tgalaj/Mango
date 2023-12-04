#pragma once
#include "imgui.h"
#include <string>

namespace mango
{
    class Font
    {
    public:
        Font();
        Font(const std::string& filename, unsigned sizeInPixels);
        ~Font() = default;

    private:
        ImFont * m_font;

        friend class ImGuiSystem;
    };
}
