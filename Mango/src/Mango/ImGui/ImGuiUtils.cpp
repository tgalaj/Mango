#include "mgpch.h"
#include "ImGuiUtils.h"
#include "Mango/Systems/ImGuiSystem.h"

#include <imgui.h>
#include <imgui_internal.h>

using namespace mango;

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

    bool TableCustomDragFloat3(const std::string& label, glm::vec3& values, float defaultValue /*= 0.0f*/, float columnWidth /*= 80.0f*/)
    {
        ImGuiIO& io       = ImGui::GetIO();
        auto     boldFont = ImGuiSystem::getFont("InterBold16");

        MG_ASSERT(boldFont);

        bool modified = false;

        ImGui::PushID(label.c_str());

        if (ImGui::BeginTable(("##TabulatedDragFloat3" + label).c_str(), 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_NoClip))
        {
            ImGui::TableSetupColumn("x", ImGuiTableColumnFlags_WidthFixed, columnWidth);
            ImGui::TableSetupColumn("y", ImGuiTableColumnFlags_WidthStretch);

            ImGui::TableNextColumn();
            ImGui::Text(label.c_str());

            ImGui::TableNextColumn();

            ImGui::BeginChild(("##Child" + label).c_str(),
                              ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetFrameHeightWithSpacing()),
                              0, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

            ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth() * 1.1f);
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

            float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
            ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

            ImGui::PushStyleColor(ImGuiCol_Button,        { 0.8f, 0.1f, 0.15f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.9f, 0.2f, 0.25f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonActive,  { 0.8f, 0.1f, 0.15f, 1.0f });
            ImGui::PushFont(boldFont);

            ImGui::PushAllowKeyboardFocus(false);
            if (ImGui::Button("X", buttonSize))
            {
                values.x  = defaultValue;
                modified |= true;
            }
            ImGui::PopAllowKeyboardFocus();

            ImGui::PopFont();
            ImGui::PopStyleColor(3);

            ImGui::SameLine();
            modified |= ImGui::DragFloat("##X", &values.x, 0.1f, 0.0f, 0.0f, "%.2f", 0);
            ImGui::PopItemWidth();
            ImGui::SameLine();

            ImGui::PushStyleColor(ImGuiCol_Button,        { 0.2f, 0.7f, 0.2f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.3f, 0.8f, 0.3f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonActive,  { 0.2f, 0.7f, 0.2f, 1.0f });
            ImGui::PushFont(boldFont);

            ImGui::PushAllowKeyboardFocus(false);
            if (ImGui::Button("Y", buttonSize))
            {
                values.y = defaultValue;
                modified |= true;
            }
            ImGui::PopAllowKeyboardFocus();

            ImGui::PopFont();
            ImGui::PopStyleColor(3);

            ImGui::SameLine();
            modified |= ImGui::DragFloat("##Y", &values.y, 0.1f, 0.0f, 0.0f, "%.2f", 0);
            ImGui::PopItemWidth();
            ImGui::SameLine();

            ImGui::PushStyleColor(ImGuiCol_Button,        { 0.1f, 0.25f, 0.8f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.2f, 0.35f, 0.9f, 1.0f });
            ImGui::PushStyleColor(ImGuiCol_ButtonActive,  { 0.1f, 0.25f, 0.8f, 1.0f });
            ImGui::PushFont(boldFont);

            ImGui::PushAllowKeyboardFocus(false);
            if (ImGui::Button("Z", buttonSize))
            {
                values.z = defaultValue;
                modified |= true;
            }
            ImGui::PopAllowKeyboardFocus();

            ImGui::PopFont();
            ImGui::PopStyleColor(3);

            ImGui::SameLine();
            modified |= ImGui::DragFloat("##Z", &values.z, 0.1f, 0.0f, 0.0f, "%.2f", 0);
            ImGui::PopItemWidth();

            ImGui::PopStyleVar();
            ImGui::EndChild();
            ImGui::EndTable();
        }

        ImGui::PopID();

        return modified;
    }

    inline void TableDrawLabelAlignedLeft(const char* label)
    {
        ImGui::TableNextColumn();
        ImGui::Text(label);

        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(-1);
    }

    bool TableBeginCombo(const char* label, const char* preview_value, ImGuiComboFlags flags /*= 0*/)
    {
        TableDrawLabelAlignedLeft(label);

        return ImGui::BeginCombo(std::format("##{}", label).c_str(), preview_value, flags);
    }

    bool TableButton(const char* label, const char* buttonText, const ImVec2& size /*= ImVec2(0, 0)*/)
    {
        TableDrawLabelAlignedLeft(label);

        ImGui::PushID(label);
        bool ret = ImGui::Button(buttonText, size);
        ImGui::PopID();

        return ret;
    }

    bool TableCheckbox(const char* label, bool* v)
    {
        TableDrawLabelAlignedLeft(label);

        ImGui::PushID(label);
        bool ret = ImGui::Checkbox("##", v);
        ImGui::PopID();

        return ret;
    }

    bool TableColorEdit3(const char* label, float col[3], ImGuiColorEditFlags flags /*= 0*/)
    {
        TableDrawLabelAlignedLeft(label);

        ImGui::PushID(label);
        bool ret = ImGui::ColorEdit3("##", col, flags);
        ImGui::PopID();

        return ret;
    }

    bool TableDragFloat(const char* label, float* v, float v_speed /*= 1.0f*/, float v_min /*= 0.0f*/, float v_max /*= 0.0f*/, const char* format /*= "%.3f"*/, ImGuiSliderFlags flags /*= 0*/)
    {
        TableDrawLabelAlignedLeft(label);

        ImGui::PushID(label);
        bool ret = ImGui::DragFloat("##", v, v_speed, v_min, v_max, format, flags);
        ImGui::PopID();

        return ret;
    }

    bool TableDragFloat3(const char* label, float v[3], float v_speed /*= 1.0f*/, float v_min /*= 0.0f*/, float v_max /*= 0.0f*/, const char* format /*= "%.3f"*/, ImGuiSliderFlags flags /*= 0*/)
    {
        TableDrawLabelAlignedLeft(label);

        ImGui::PushID(label);
        bool ret = ImGui::DragFloat3("##", v, v_speed, v_min, v_max, format, flags);
        ImGui::PopID();

        return ret;
    }
}