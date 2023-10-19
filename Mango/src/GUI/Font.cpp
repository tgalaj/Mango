#include "mgpch.h"
#include "Font.h"

mango::Font::Font()
{
    m_font = ImGui::GetIO().Fonts->AddFontDefault();
}

mango::Font::Font(const std::filesystem::path& filepath, unsigned sizeInPixels)
{
    m_font = ImGui::GetIO().Fonts->AddFontFromFileTTF(filepath.string().c_str(), sizeInPixels);
}
