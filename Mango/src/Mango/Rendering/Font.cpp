#include "mgpch.h"
#include "Font.h"

mango::Font::Font()
{
    MG_PROFILE_ZONE_SCOPED;

    m_font = ImGui::GetIO().Fonts->AddFontDefault();
}

mango::Font::Font(const std::filesystem::path& filepath, unsigned sizeInPixels)
{
    MG_PROFILE_ZONE_SCOPED;

    m_font = ImGui::GetIO().Fonts->AddFontFromFileTTF(filepath.string().c_str(), sizeInPixels);
}
