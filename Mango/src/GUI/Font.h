#pragma once
#include "imgui.h"
#include <filesystem>
#include <string>

namespace mango
{
    class Font
    {
    public:
        Font();
        Font(const std::filesystem::path& filepath, unsigned sizeInPixels);
        ~Font() = default;

    private:
        ImFont * m_font;

        friend class GUI;
    };
}
