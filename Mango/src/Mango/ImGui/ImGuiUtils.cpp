#include "mgpch.h"
#include "ImGuiUtils.h"

#include <imgui.h>

namespace ImGui::Utils
{
    void AlignWidget(const char* label, float alignment /*= 0.5f*/)
    {
        ImGuiStyle& style = ImGui::GetStyle();

        float size  = ImGui::CalcTextSize(label).x + style.FramePadding.x * 2.0f;
        float avail = ImGui::GetContentRegionAvail().x;

        float off = (avail - size) * alignment;
        if (off > 0.0f) ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);
    }

    bool ButtonAligned(const char* label, float alignment /*= 0.5f*/)
    {
        AlignWidget(label, alignment);

        return ImGui::Button(label);
    }
}