#include "framework/gui/Font.h"


Vertex::Font::Font()
{
    m_font = ImGui::GetIO().Fonts->AddFontDefault();
}

Vertex::Font::Font(const std::string& filepathname, unsigned size_pixels)
{
    m_font = ImGui::GetIO().Fonts->AddFontFromFileTTF(filepathname.c_str(), size_pixels);
}
