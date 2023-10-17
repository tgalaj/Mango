#include "mgpch.h"

#include "gui/Font.h"


mango::Font::Font()
{
    m_font = ImGui::GetIO().Fonts->AddFontDefault();
}

mango::Font::Font(const std::string& filepathname, unsigned size_pixels)
{
    m_font = ImGui::GetIO().Fonts->AddFontFromFileTTF(filepathname.c_str(), size_pixels);
}
