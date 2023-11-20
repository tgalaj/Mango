﻿#include "mgpch.h"

#include "ImGuiSystem.h"
#include "Mango/Core/Services.h"
#include "Mango/Rendering/Font.h"
#include "Mango/Rendering/Texture.h"
#include "Mango/Window/Window.h"

#include "glm/vec2.hpp"
#include "glm/common.hpp"
#include "imgui_internal.h"
#include "Mango/ImGui/imgui_impl_glfw.h"
#include "Mango/ImGui/imgui_impl_opengl3.h"

namespace mango
{
    glm::vec2 ImGuiSystem::m_windowSize = glm::vec2(0.0f);

    ImGuiSystem::ImGuiSystem()
        : System("ImGuiSystem")
    {

    }

    void ImGuiSystem::onInit()
    {
        auto app = Services::application();
        MG_CORE_ASSERT_MSG(app != nullptr, "app can't be nullptr!");

        auto window = app->getWindow();
        MG_CORE_ASSERT_MSG(window != nullptr, "window can't be nullptr!");

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGui_ImplGlfw_InitForOpenGL(window->getNativeWindow(), true);
        ImGui_ImplOpenGL3_Init("#version 460");

        m_windowSize = glm::vec2(window->getWidth(), window->getHeight());

        ImGui::GetIO().Fonts->AddFontDefault();
    }

    void ImGuiSystem::onDestroy()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void ImGuiSystem::being()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void ImGuiSystem::end()
    {
        glViewport(0, 0, GLsizei(m_windowSize.x), GLsizei(m_windowSize.y));
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void ImGuiSystem::updateWindowSize(float width, float height)
    {
        m_windowSize = glm::vec2(width, height);
    }

    void ImGuiSystem::beginHUD()
    {
        ImGuiIO& io = ImGui::GetIO();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
        ImGui::PushStyleColor(ImGuiCol_WindowBg, { 0.0f, 0.0f, 0.0f, 0.0f });
        ImGui::Begin("##Backbuffer", nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_NoFocusOnAppearing|ImGuiWindowFlags_NoBringToFrontOnFocus);

        ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
        ImGui::SetWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y), ImGuiCond_Always);
    }

    void ImGuiSystem::endHUD()
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        window->DrawList->PushClipRectFullScreen();

        ImGui::End();
        ImGui::PopStyleColor();
        ImGui::PopStyleVar(2);
    }

    float ImGuiSystem::text(const std::shared_ptr<Font> & font, const std::string& text, const glm::vec2 & position, float size, const glm::vec4 & color /*= glm::vec4(1.0f)*/, bool center /*= false*/, bool textShadow /*= false*/)
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();

        std::stringstream stream(text);
        std::string line;

        const auto textColor = glm::clamp(color, 0.0f, 1.0f);

        float y = 0.0f;
        int   i = 0;

        while (std::getline(stream, line))
        {
            const auto textSize = font->m_font->CalcTextSizeA(size, FLT_MAX, 0.0f, line.c_str());

            if (center)
            {
                if(textShadow)
                {
                    window->DrawList->AddText(font->m_font, size, { (position.x - textSize.x / 2.0f) + 1.0f, (position.y + textSize.y * i) + 1.0f }, ImGui::GetColorU32({ 0.0f, 0.0f, 0.0f, textColor.a }), line.c_str());
                    window->DrawList->AddText(font->m_font, size, { (position.x - textSize.x / 2.0f) - 1.0f, (position.y + textSize.y * i) - 1.0f }, ImGui::GetColorU32({ 0.0f, 0.0f, 0.0f, textColor.a }), line.c_str());
                    window->DrawList->AddText(font->m_font, size, { (position.x - textSize.x / 2.0f) + 1.0f, (position.y + textSize.y * i) - 1.0f }, ImGui::GetColorU32({ 0.0f, 0.0f, 0.0f, textColor.a }), line.c_str());
                    window->DrawList->AddText(font->m_font, size, { (position.x - textSize.x / 2.0f) - 1.0f, (position.y + textSize.y * i) + 1.0f }, ImGui::GetColorU32({ 0.0f, 0.0f, 0.0f, textColor.a }), line.c_str());
                }

                window->DrawList->AddText(font->m_font, size, { position.x - textSize.x / 2.0f, position.y + textSize.y * i }, ImGui::GetColorU32({ textColor.r, textColor.g, textColor.b, textColor.a }), line.c_str());
            }
            else
            {
                if(textShadow)
                {
                    window->DrawList->AddText(font->m_font, size, { (position.x) + 1.0f, (position.y + textSize.y * i) + 1.0f }, ImGui::GetColorU32({ 0.0f, 0.0f, 0.0f, textColor.a }), line.c_str());
                    window->DrawList->AddText(font->m_font, size, { (position.x) - 1.0f, (position.y + textSize.y * i) - 1.0f }, ImGui::GetColorU32({ 0.0f, 0.0f, 0.0f, textColor.a }), line.c_str());
                    window->DrawList->AddText(font->m_font, size, { (position.x) + 1.0f, (position.y + textSize.y * i) - 1.0f }, ImGui::GetColorU32({ 0.0f, 0.0f, 0.0f, textColor.a }), line.c_str());
                    window->DrawList->AddText(font->m_font, size, { (position.x) - 1.0f, (position.y + textSize.y * i) + 1.0f }, ImGui::GetColorU32({ 0.0f, 0.0f, 0.0f, textColor.a }), line.c_str());
                }

                window->DrawList->AddText(font->m_font, size, { position.x, position.y + textSize.y * i }, ImGui::GetColorU32({ textColor.r, textColor.g, textColor.b, textColor.a }), line.c_str());
            }

            y = position.y + textSize.y * (i + 1);
            i++;
        }

        return y;
    }

    void ImGuiSystem::line(const glm::vec2& from, const glm::vec2& to, const glm::vec4& color, float thickness)
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();

        const auto lineColor = glm::clamp(color, 0.0f, 1.0f);

        window->DrawList->AddLine({ from.x, from.y }, { to.x, to.y }, ImGui::GetColorU32({ lineColor.r, lineColor.g, lineColor.b, lineColor.a }), thickness);
    }

    void ImGuiSystem::circle(const glm::vec2& position, float radius, const glm::vec4& color, float thickness, uint32_t segments)
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();

        const auto circleColor = glm::clamp(color, 0.0f, 1.0f);

        window->DrawList->AddCircle({ position.x, position.y }, radius, ImGui::GetColorU32({ circleColor.r, circleColor.g, circleColor.b, circleColor.a }), segments, thickness);
    }

    void ImGuiSystem::circleFilled(const glm::vec2& position, float radius,  const glm::vec4& color, uint32_t segments)
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();

        const auto circleColor = glm::clamp(color, 0.0f, 1.0f);

        window->DrawList->AddCircleFilled({ position.x, position.y }, radius, ImGui::GetColorU32({ circleColor.r, circleColor.g, circleColor.b, circleColor.a }), segments);
    }

    void ImGuiSystem::rect(const glm::vec2& from, const glm::vec2& to,  const glm::vec4& color, float rounding, uint32_t roundingCornersFlags, float thickness)
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();

        const auto rectColor = glm::clamp(color, 0.0f, 1.0f);

        window->DrawList->AddRect({ from.x, from.y }, { to.x, to.y }, ImGui::GetColorU32({ rectColor.r, rectColor.g, rectColor.b, rectColor.a }), rounding, roundingCornersFlags, thickness);
    }

    void ImGuiSystem::rectFilled(const glm::vec2& from, const glm::vec2& to,  const glm::vec4& color, float rounding, uint32_t roundingCornersFlags)
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();

        const auto rectColor = glm::clamp(color, 0.0f, 1.0f);

        window->DrawList->AddRectFilled({ from.x, from.y }, { to.x, to.y }, ImGui::GetColorU32({ rectColor.r, rectColor.g, rectColor.b, rectColor.a }), rounding, roundingCornersFlags);
    }

    void ImGuiSystem::image(std::shared_ptr<Texture> pTexture, const glm::vec2& from, const glm::vec2& to,  const glm::vec4& color)
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();

        const auto tintColor = glm::clamp(color, 0.0f, 1.0f);

        window->DrawList->AddImage(reinterpret_cast<void*>(pTexture->m_id), { from.x, from.y }, { to.x, to.y }, { 0.0f, 0.0f }, { 1.0f, 1.0f }, ImGui::GetColorU32({ tintColor.r, tintColor.g, tintColor.b, tintColor.a }));
    }
    
    void ImGuiSystem::imageRounded(std::shared_ptr<Texture> pTexture, const glm::vec2& from, const glm::vec2& to,  const glm::vec4& color, float rounding, uint32_t roundingCornersFlags)
    {
        ImGuiWindow* window = ImGui::GetCurrentWindow();

        const auto tintColor = glm::clamp(color, 0.0f, 1.0f);

        window->DrawList->AddImageRounded(reinterpret_cast<void*>(pTexture->m_id), { from.x, from.y }, { to.x, to.y }, { 0.0f, 0.0f }, { 1.0f, 1.0f }, ImGui::GetColorU32({ tintColor.r, tintColor.g, tintColor.b, tintColor.a }), rounding, roundingCornersFlags);
    }
}
