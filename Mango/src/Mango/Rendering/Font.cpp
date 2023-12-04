#include "mgpch.h"
#include "Font.h"

mango::Font::Font()
{
    MG_PROFILE_ZONE_SCOPED;

    m_font = ImGui::GetIO().Fonts->AddFontDefault();
}

mango::Font::Font(const std::string& filename, unsigned sizeInPixels)
{
    MG_PROFILE_ZONE_SCOPED;

    auto filepath = VFI::getFilepath(filename);

    m_font = ImGui::GetIO().Fonts->AddFontFromFileTTF(filepath.string().c_str(), sizeInPixels);
}
