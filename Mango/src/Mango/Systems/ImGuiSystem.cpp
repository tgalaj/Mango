#include "mgpch.h"

#include "ImGuiSystem.h"
#include "Mango/Core/Services.h"
#include "Mango/Rendering/Font.h"
#include "Mango/Rendering/Texture.h"
#include "Mango/Window/Window.h"

#include "glm/vec2.hpp"
#include "glm/common.hpp"
#include "imgui_internal.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "ImGuizmo.h"

namespace mango
{
    glm::vec2                                ImGuiSystem::m_windowSize = glm::vec2(0.0f);
    std::unordered_map<std::string, ImFont*> ImGuiSystem::s_ImFonts;

    ImGuiSystem::ImGuiSystem()
        : System("ImGuiSystem")
    {

    }

    void ImGuiSystem::onInit()
    {
        MG_PROFILE_ZONE_SCOPED;

        auto app = Services::application();
        MG_CORE_ASSERT_MSG(app != nullptr, "app can't be nullptr!");

        auto window = app->getWindow();
        MG_CORE_ASSERT_MSG(window != nullptr, "window can't be nullptr!");

        m_windowSize = glm::vec2(window->getWidth(), window->getHeight());
        
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
        ImGuiStyle& style = ImGui::GetStyle();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding              = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }
        setDarkThemeColors();

        // Set ImGuizmo style
        ImGuizmo::Style& imguizmoStyle = ImGuizmo::GetStyle();
        imguizmoStyle.Colors[ImGuizmo::COLOR::DIRECTION_X] = { 0.847059f, 0.368627f, 0.443137f, 1.0f };
        imguizmoStyle.Colors[ImGuizmo::COLOR::DIRECTION_Y] = { 0.145098f, 0.666667f, 0.145098f, 1.0f };
        imguizmoStyle.Colors[ImGuizmo::COLOR::DIRECTION_Z] = { 0.172549f, 0.329411f, 0.8f,      1.0f };

        imguizmoStyle.Colors[ImGuizmo::COLOR::PLANE_X] = { 0.847059f, 0.407843f, 0.482352f, 1.0f };
        imguizmoStyle.Colors[ImGuizmo::COLOR::PLANE_Y] = { 0.333333f, 0.670588f, 0.333333f, 1.0f };
        imguizmoStyle.Colors[ImGuizmo::COLOR::PLANE_Z] = { 0.258823f, 0.407843f, 0.850980f, 1.0f };

        imguizmoStyle.Colors[ImGuizmo::COLOR::SELECTION] = { 0.8f, 0.666667f, 0.125490f, 1.0f };

        imguizmoStyle.TranslationLineThickness   = 5.0f;
        imguizmoStyle.TranslationLineArrowSize   = imguizmoStyle.TranslationLineThickness * 2.0f;
        imguizmoStyle.RotationLineThickness      = 5.0f;
        imguizmoStyle.RotationOuterLineThickness = imguizmoStyle.RotationLineThickness + 1.0f;
        imguizmoStyle.ScaleLineThickness         = 5.0f;
        imguizmoStyle.ScaleLineCircleSize        = imguizmoStyle.ScaleLineThickness * 2.0f;
        imguizmoStyle.HatchedAxisLineThickness   = 10.0f;
        imguizmoStyle.CenterCircleSize           = 10.0f;

        ImGui_ImplGlfw_InitForOpenGL(window->getNativeWindow(), true);
        ImGui_ImplOpenGL3_Init("#version 460");
    }

    void ImGuiSystem::onDestroy()
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("ImGuiSystem::onDestroy");

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void ImGuiSystem::being()
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("ImGuiSystem::begin");

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGuizmo::BeginFrame();
    }

    void ImGuiSystem::end()
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("ImGuiSystem::end");

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        //glViewport(0, 0, GLsizei(m_windowSize.x), GLsizei(m_windowSize.y));
        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2(Services::application()->getWindow()->getWidth(), Services::application()->getWindow()->getHeight());

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }

    void ImGuiSystem::updateWindowSize(float width, float height)
    {
        m_windowSize = glm::vec2(width, height);
    }

    void ImGuiSystem::addFont(const std::string& name, const std::string& path, float size, bool isDefault /*= false*/)
    {
        float dpiScale = Services::application()->getWindow()->getDpiScale().x;

        if (size <= 0.0f) size = 16.0f;

        size = std::floorf(size * dpiScale);

        ImGuiIO& io = ImGui::GetIO();

        if (isDefault)
        {
            io.FontDefault  = io.Fonts->AddFontFromFileTTF(VFI::getFilepath(path).string().c_str(), size);
            s_ImFonts[name] = io.FontDefault;
        }
        else
        {
            auto font       = io.Fonts->AddFontFromFileTTF(VFI::getFilepath(path).string().c_str(), size);
            s_ImFonts[name] = font;
        }

        ImGuiStyle& style = ImGui::GetStyle();
        style.ScaleAllSizes(dpiScale);
    }

    void ImGuiSystem::mergeFont(const std::string& path, float size, const ImWchar iconRanges[3])
    {
        float dpiScale = Services::application()->getWindow()->getDpiScale().x;

        if (size <= 0.0f) size = 16.0f;

        size = std::floorf(size * dpiScale);

        ImGuiIO& io = ImGui::GetIO();

        ImFontConfig config;
        config.MergeMode        = true;
        config.PixelSnapH = true;
        config.GlyphMinAdvanceX = size; // Use if you want to make the icon monospaced

        io.Fonts->AddFontFromFileTTF(VFI::getFilepath(path).string().c_str(), size, &config, iconRanges);
    }

    void ImGuiSystem::setDefaultIniSettingsFile(const std::string& filename)
    {
        m_iniFilepath = VFI::getFilepath(filename).string();

        ImGui::GetIO().IniFilename = m_iniFilepath.c_str();
    }

    void ImGuiSystem::beginHUD()
    {
        MG_PROFILE_ZONE_SCOPED;

        ImGuiIO& io = ImGui::GetIO();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y));
        ImGui::Begin("##Backbuffer", nullptr, ImGuiWindowFlags_NoTitleBar         |
                                              ImGuiWindowFlags_NoResize           |
                                              ImGuiWindowFlags_NoMove             |
                                              ImGuiWindowFlags_NoInputs           |
                                              ImGuiWindowFlags_NoBackground       |
                                              ImGuiWindowFlags_NoScrollbar        |
                                              ImGuiWindowFlags_NoSavedSettings    |
                                              ImGuiWindowFlags_NoFocusOnAppearing |
                                              ImGuiWindowFlags_NoBringToFrontOnFocus);

    }

    void ImGuiSystem::endHUD()
    {
        MG_PROFILE_ZONE_SCOPED;

        ImGuiWindow* window = ImGui::GetCurrentWindow();
        window->DrawList->PushClipRectFullScreen();

        ImGui::End();
        ImGui::PopStyleVar(2);
    }

    float ImGuiSystem::text(const ref<Font> & font, const std::string& text, const glm::vec2 & position, float size, const glm::vec4 & color /*= glm::vec4(1.0f)*/, bool center /*= false*/, bool textShadow /*= false*/)
    {
        MG_PROFILE_ZONE_SCOPED;
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
        MG_PROFILE_ZONE_SCOPED;
        ImGuiWindow* window = ImGui::GetCurrentWindow();

        const auto lineColor = glm::clamp(color, 0.0f, 1.0f);

        window->DrawList->AddLine({ from.x, from.y }, { to.x, to.y }, ImGui::GetColorU32({ lineColor.r, lineColor.g, lineColor.b, lineColor.a }), thickness);
    }

    void ImGuiSystem::circle(const glm::vec2& position, float radius, const glm::vec4& color, float thickness, uint32_t segments)
    {
        MG_PROFILE_ZONE_SCOPED;
        ImGuiWindow* window = ImGui::GetCurrentWindow();

        const auto circleColor = glm::clamp(color, 0.0f, 1.0f);

        window->DrawList->AddCircle({ position.x, position.y }, radius, ImGui::GetColorU32({ circleColor.r, circleColor.g, circleColor.b, circleColor.a }), segments, thickness);
    }

    void ImGuiSystem::circleFilled(const glm::vec2& position, float radius,  const glm::vec4& color, uint32_t segments)
    {
        MG_PROFILE_ZONE_SCOPED;
        ImGuiWindow* window = ImGui::GetCurrentWindow();

        const auto circleColor = glm::clamp(color, 0.0f, 1.0f);

        window->DrawList->AddCircleFilled({ position.x, position.y }, radius, ImGui::GetColorU32({ circleColor.r, circleColor.g, circleColor.b, circleColor.a }), segments);
    }

    void ImGuiSystem::rect(const glm::vec2& from, const glm::vec2& to,  const glm::vec4& color, float rounding, uint32_t roundingCornersFlags, float thickness)
    {
        MG_PROFILE_ZONE_SCOPED;
        ImGuiWindow* window = ImGui::GetCurrentWindow();

        const auto rectColor = glm::clamp(color, 0.0f, 1.0f);

        window->DrawList->AddRect({ from.x, from.y }, { to.x, to.y }, ImGui::GetColorU32({ rectColor.r, rectColor.g, rectColor.b, rectColor.a }), rounding, roundingCornersFlags, thickness);
    }

    void ImGuiSystem::rectFilled(const glm::vec2& from, const glm::vec2& to,  const glm::vec4& color, float rounding, uint32_t roundingCornersFlags)
    {
        MG_PROFILE_ZONE_SCOPED;
        ImGuiWindow* window = ImGui::GetCurrentWindow();

        const auto rectColor = glm::clamp(color, 0.0f, 1.0f);

        window->DrawList->AddRectFilled({ from.x, from.y }, { to.x, to.y }, ImGui::GetColorU32({ rectColor.r, rectColor.g, rectColor.b, rectColor.a }), rounding, roundingCornersFlags);
    }

    void ImGuiSystem::image(ref<Texture> pTexture, const glm::vec2& from, const glm::vec2& to,  const glm::vec4& color)
    {
        MG_PROFILE_ZONE_SCOPED;
        ImGuiWindow* window = ImGui::GetCurrentWindow();

        const auto tintColor = glm::clamp(color, 0.0f, 1.0f);

        window->DrawList->AddImage((ImTextureID)pTexture->getRendererID(), { from.x, from.y }, { to.x, to.y }, { 0.0f, 0.0f }, { 1.0f, 1.0f }, ImGui::GetColorU32({ tintColor.r, tintColor.g, tintColor.b, tintColor.a }));
    }
    
    void ImGuiSystem::imageRounded(ref<Texture> pTexture, const glm::vec2& from, const glm::vec2& to,  const glm::vec4& color, float rounding, uint32_t roundingCornersFlags)
    {
        MG_PROFILE_ZONE_SCOPED;
        ImGuiWindow* window = ImGui::GetCurrentWindow();

        const auto tintColor = glm::clamp(color, 0.0f, 1.0f);

        window->DrawList->AddImageRounded((ImTextureID)pTexture->getRendererID(), { from.x, from.y }, { to.x, to.y }, { 0.0f, 0.0f }, { 1.0f, 1.0f }, ImGui::GetColorU32({ tintColor.r, tintColor.g, tintColor.b, tintColor.a }), rounding, roundingCornersFlags);
    }

    void ImGuiSystem::setDarkThemeColors()
    {
        auto& colors              = ImGui::GetStyle().Colors;
        colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

        // Headers
        colors[ImGuiCol_Header]        = ImVec4{ 0.2f,  0.205f,  0.21f,  1.0f };
        colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f,  0.305f,  0.31f,  1.0f };
        colors[ImGuiCol_HeaderActive]  = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

        // Buttons
        colors[ImGuiCol_Button]        = ImVec4{ 0.2f,  0.205f,  0.21f,  1.0f };
        colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f,  0.305f,  0.31f,  1.0f };
        colors[ImGuiCol_ButtonActive]  = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

        // Frame BG
        colors[ImGuiCol_FrameBg]        = ImVec4{ 0.2f,  0.205f,  0.21f,  1.0f };
        colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f,  0.305f,  0.31f,  1.0f };
        colors[ImGuiCol_FrameBgActive]  = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

        // Tabs
        colors[ImGuiCol_Tab]                = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
        colors[ImGuiCol_TabHovered]         = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
        colors[ImGuiCol_TabActive]          = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
        colors[ImGuiCol_TabUnfocused]       = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f,  0.205f,  0.21f,  1.0f };

        // Title
        colors[ImGuiCol_TitleBg]          = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
        colors[ImGuiCol_TitleBgActive]    = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
    }
}