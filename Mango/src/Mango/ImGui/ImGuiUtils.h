#pragma once

namespace ImGui::Utils
{
    void AlignWidget  (const char* label, float alignment = 0.5f);
    bool ButtonAligned(const char* label, float alignment = 0.5f);

    bool TableCustomDragFloat3(const std::string& label, glm::vec3& values, float defaultValue = 0.0f, float columnWidth = 80.0f);

    bool TableBeginCombo(const char* label, const char* preview_value, ImGuiComboFlags flags = 0);
    bool TableButton(const char* label, const char* buttonText, const ImVec2& size = ImVec2(0, 0));
    bool TableCheckbox(const char* label, bool* v);
    bool TableColorEdit3(const char* label, float col[3], ImGuiColorEditFlags flags = 0);
    bool TableDragFloat(const char* label, float* v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0);     // If v_min >= v_max we have no bound
    bool TableDragFloat3(const char* label, float v[3], float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* format = "%.3f", ImGuiSliderFlags flags = 0);

}