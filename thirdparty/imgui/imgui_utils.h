#pragma once

#include "IconsFontAwesome.h" // from https://github.com/juliettef/IconFontCppHeaders
#include "imgui.h"

#include <cmath>
#include <string>

namespace ImGui
{
    inline void SetImGuiFont(const std::string & font_name, float font_size)
    {
        ImGuiIO& io = ImGui::GetIO();
        std::string font_dir = "res/fonts/" + font_name;

        io.Fonts->AddFontFromFileTTF(font_dir.c_str(), font_size);
    }

    inline void SetupImGuiStyle(void (*style_fun)(ImGuiStyle &), float alpha_, bool bStyleDark_ = false)
    {
        ImGuiStyle & style = ImGui::GetStyle();

        style_fun(style);

        if (bStyleDark_)
        {
            for (int i = 0; i <= ImGuiCol_COUNT; i++)
            {
                ImVec4& col = style.Colors[i];
                float H, S, V;
                ImGui::ColorConvertRGBtoHSV(col.x, col.y, col.z, H, S, V);

                if (S < 0.1f)
                {
                    V = 1.0f - V;
                }
                ImGui::ColorConvertHSVtoRGB(H, S, V, col.x, col.y, col.z);
                if (col.w < 1.00f)
                {
                    col.w *= alpha_;
                }
            }
        }
        else
        {
            for (int i = 0; i <= ImGuiCol_COUNT; i++)
            {
                ImVec4& col = style.Colors[i];
                if (col.w < 1.00f)
                {
                    col.x *= alpha_;
                    col.y *= alpha_;
                    col.z *= alpha_;
                    col.w *= alpha_;
                }
            }
        }
    }

    inline bool CheckBoxFont(const char* name_, bool* pB_, const char* pOn_ = "[X]", const char* pOff_ = "[  ]")
    {
        if (*pB_)
        {
            ImGui::Text(pOn_);
        }
        else
        {
            ImGui::Text(pOff_);
        }
        bool bHover = false;
        bHover = bHover || ImGui::IsItemHovered();
        ImGui::SameLine();
        ImGui::Text(name_);
        bHover = bHover || ImGui::IsItemHovered();
        if (bHover && ImGui::IsMouseClicked(0))
        {
            *pB_ = !*pB_;
            return true;
        }
        return false;
    }

    inline bool CheckBoxTick(const char* name_, bool* pB_)
    {
        return CheckBoxFont(name_, pB_, ICON_FA_CHECK_SQUARE_O, ICON_FA_SQUARE_O);
    }

    inline bool MenuItemCheckbox(const char* name_, bool* pB_)
    {
        bool retval = ImGui::MenuItem(name_);
        ImGui::SameLine();
        if (*pB_)
        {
            ImGui::Text(ICON_FA_CHECK_SQUARE_O);
        }
        else
        {
            ImGui::Text(ICON_FA_SQUARE_O);
        }
        if (retval)
        {
            *pB_ = !*pB_;
        }
        return retval;
    }

    struct FrameTimeHistogram
    {
        // configuration params - modify these at will
        static const int NUM = 101; //last value is from T-1 to inf.

        float  dT = 0.001f;    // in seconds, default 1ms
        float  refresh = 1.0f / 60.0f;// set this to your target refresh rate

        static const int NUM_MARKERS = 2;
        float  markers[NUM_MARKERS] = { 0.99f, 0.999f };

        // data
        ImVec2 size = ImVec2(3.0f * NUM, 40.0f);
        float  lastdT = 0.0f;
        float  timesTotal;
        float  countsTotal;
        float  times[NUM];
        float  counts[NUM];
        float  hitchTimes[NUM];
        float  hitchCounts[NUM];

        FrameTimeHistogram()
        {
            Clear();
        }

        void Clear()
        {
            timesTotal = 0.0f;
            countsTotal = 0.0f;
            memset(times, 0, sizeof(times));
            memset(counts, 0, sizeof(counts));
            memset(hitchTimes, 0, sizeof(hitchTimes));
            memset(hitchCounts, 0, sizeof(hitchCounts));
        }

        int GetBin(float time_)
        {
            int bin = (int)floor(time_ / dT);
            if (bin >= NUM)
            {
                bin = NUM - 1;
            }
            return bin;
        }

        void Update(float deltaT_)
        {
            if (deltaT_ < 0.0f)
            {
                assert(false);
                return;
            }
            int bin = GetBin(deltaT_);
            times[bin] += deltaT_;
            timesTotal += deltaT_;
            counts[bin] += 1.0f;
            countsTotal += 1.0f;

            float hitch = abs(lastdT - deltaT_);
            int deltaBin = GetBin(hitch);
            hitchTimes[deltaBin] += hitch;
            hitchCounts[deltaBin] += 1.0f;
            lastdT = deltaT_;
        }

        void PlotRefreshLines(float total_ = 0.0f, float* pValues_ = NULL)
        {
            ImDrawList* draw = ImGui::GetWindowDrawList();
            const ImGuiStyle& style = ImGui::GetStyle();
            ImVec2 pad = style.FramePadding;
            ImVec2 min = ImGui::GetItemRectMin();
            min.x += pad.x;
            ImVec2 max = ImGui::GetItemRectMax();
            max.x -= pad.x;

            float xRefresh = (max.x - min.x) * refresh / (dT * NUM);

            float xCurr = xRefresh + min.x;
            while (xCurr < max.x)
            {
                float xP = ceil(xCurr); // use ceil to get integer coords or else lines look odd
                draw->AddLine(ImVec2(xP, min.y), ImVec2(xP, max.y), 0x50FFFFFF);
                xCurr += xRefresh;
            }

            if (pValues_)
            {
                // calc markers
                float currTotal = 0.0f;
                int   mark = 0;
                for (int i = 0; i < NUM && mark < NUM_MARKERS; ++i)
                {
                    currTotal += pValues_[i];
                    if (total_ * markers[mark] < currTotal)
                    {
                        float xP = ceil((float)(i + 1) / (float)NUM * (max.x - min.x) + min.x); // use ceil to get integer coords or else lines look odd
                        draw->AddLine(ImVec2(xP, min.y), ImVec2(xP, max.y), 0xFFFF0000);
                        ++mark;
                    }
                }
            }
        }

        void CalcHistogramSize(int numShown_)
        {
            ImVec2 wRegion = ImGui::GetContentRegionMax();
            float heightGone = 7.0f * ImGui::GetItemsLineHeightWithSpacing();
            wRegion.y -= heightGone;
            wRegion.y /= (float)numShown_;
            const ImGuiStyle& style = ImGui::GetStyle();
            ImVec2 pad = style.FramePadding;
            wRegion.x -= 2.0f * pad.x;
            size = wRegion;
        }


        void Draw(const char* name_, bool* pOpen_ = NULL)
        {
            if (ImGui::Begin(name_, pOpen_))
            {
                int numShown = 0;
                if (ImGui::CollapsingHeader("Time Histogram"))
                {
                    ++numShown;
                    ImGui::PlotHistogram("", times, NUM, 0, NULL, FLT_MAX, FLT_MAX, size);
                    PlotRefreshLines(timesTotal, times);
                }
                if (ImGui::CollapsingHeader("Count Histogram"))
                {
                    ++numShown;
                    ImGui::PlotHistogram("", counts, NUM, 0, NULL, FLT_MAX, FLT_MAX, size);
                    PlotRefreshLines(countsTotal, counts);
                }
                if (ImGui::CollapsingHeader("Hitch Time Histogram"))
                {
                    ++numShown;
                    ImGui::PlotHistogram("", hitchTimes, NUM, 0, NULL, FLT_MAX, FLT_MAX, size);
                    PlotRefreshLines();
                }
                if (ImGui::CollapsingHeader("Hitch Count Histogram"))
                {
                    ++numShown;
                    ImGui::PlotHistogram("", hitchCounts, NUM, 0, NULL, FLT_MAX, FLT_MAX, size);
                    PlotRefreshLines();
                }
                if (ImGui::Button("Clear"))
                {
                    Clear();
                }
                CalcHistogramSize(numShown);
            }
            ImGui::End();
        }
    };
};

namespace GUIStyle
{
    inline void RayTeak(ImGuiStyle & style)
    {
        // RayTeak style from https://www.unknowncheats.me/forum/direct3d/189635-imgui-style-settings.html
        ImGui::SetImGuiFont("Ruda-Bold.ttf", 14.0f);

        style.WindowPadding = ImVec2(15, 15);
        style.WindowRounding = 5.0f;
        style.FramePadding = ImVec2(5, 5);
        style.FrameRounding = 4.0f;
        style.ItemSpacing = ImVec2(12, 8);
        style.ItemInnerSpacing = ImVec2(8, 6);
        style.IndentSpacing = 25.0f;
        style.ScrollbarSize = 15.0f;
        style.ScrollbarRounding = 9.0f;
        style.GrabMinSize = 5.0f;
        style.GrabRounding = 3.0f;

        style.Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
        style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
        style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
        style.Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
        style.Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
        style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
        style.Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
        style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
        style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
        style.Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
        style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
        style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
        style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
        style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
        style.Colors[ImGuiCol_ComboBg] = ImVec4(0.19f, 0.18f, 0.21f, 1.00f);
        style.Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
        style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
        style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
        style.Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
        style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
        style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
        style.Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
        style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
        style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
        style.Colors[ImGuiCol_Column] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
        style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
        style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
        style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
        style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
        style.Colors[ImGuiCol_CloseButton] = ImVec4(0.40f, 0.39f, 0.38f, 0.16f);
        style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.40f, 0.39f, 0.38f, 0.39f);
        style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.40f, 0.39f, 0.38f, 1.00f);
        style.Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
        style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
        style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
        style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
        style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
        style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);
    }

    inline void Maya(ImGuiStyle & style)
    {
        ImGui::SetImGuiFont("Roboto-Regular.ttf", 16.0f);

        // Maya style from https://gist.github.com/ongamex/4ee36fb23d6c527939d0f4ba72144d29
        style.ChildWindowRounding = 3.f;
        style.GrabRounding = 0.f;
        style.WindowRounding = 0.f;
        style.ScrollbarRounding = 3.f;
        style.FrameRounding = 3.f;
        style.WindowTitleAlign = ImVec2(0.5f, 0.5f);

        style.Colors[ImGuiCol_Text] = ImVec4(0.73f, 0.73f, 0.73f, 1.00f);
        style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.26f, 0.26f, 0.26f, 0.95f);
        style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
        style.Colors[ImGuiCol_PopupBg] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
        style.Colors[ImGuiCol_Border] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
        style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
        style.Colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
        style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
        style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
        style.Colors[ImGuiCol_TitleBg] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
        style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
        style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
        style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
        style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
        style.Colors[ImGuiCol_ComboBg] = ImVec4(0.32f, 0.32f, 0.32f, 1.00f);
        style.Colors[ImGuiCol_CheckMark] = ImVec4(0.78f, 0.78f, 0.78f, 1.00f);
        style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.74f, 0.74f, 0.74f, 1.00f);
        style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.74f, 0.74f, 0.74f, 1.00f);
        style.Colors[ImGuiCol_Button] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
        style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.43f, 0.43f, 0.43f, 1.00f);
        style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
        style.Colors[ImGuiCol_Header] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
        style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
        style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
        style.Colors[ImGuiCol_Column] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
        style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.36f, 0.36f, 0.36f, 1.00f);
        style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        style.Colors[ImGuiCol_CloseButton] = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
        style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
        style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
        style.Colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
        style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
        style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
        style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.32f, 0.52f, 0.65f, 1.00f);
        style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.50f);
    }
}