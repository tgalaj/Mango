﻿#pragma once

#include <imgui.h>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <memory>

#include "Font.h"
#include "imgui_impl_glfw.h"
#include "Mango/Rendering/Texture.h"
#include "Mango/Window/Window.h"

namespace mango
{
    class GUI
    {
    public:
        using Style = void(*)(ImGuiStyle &);

        ~GUI();
        static void init(GLFWwindow * window);
        static void prepare();
        static void render();
        static void updateWindowSize(float width, float height);

        /* HUD rendering */
        static void beginHUD();
        static void endHUD();

        static float text(const std::shared_ptr<Font> & font, const std::string& text, const glm::vec2 & position, float size, const glm::vec4 & color = glm::vec4(1.0f), bool center = false, bool textShadow = false);
        static void line(const glm::vec2 & from, const glm::vec2 & to, const glm::vec4 & color = glm::vec4(1.0f), float thickness = 1.0f);
        static void circle(const glm::vec2 & position, float radius, const glm::vec4 & color = glm::vec4(1.0f), float thickness = 1.0f, uint32_t segments = 16);
        static void circleFilled(const glm::vec2 & position, float radius, const glm::vec4 & color = glm::vec4(1.0f), uint32_t segments = 16);
        static void rect(const glm::vec2 & from, const glm::vec2 & to, const glm::vec4 & color = glm::vec4(1.0f), float rounding = 0.0f, uint32_t roundingCornersFlags = ImDrawCornerFlags_All, float thickness = 1.0f);
        static void rectFilled(const glm::vec2 & from, const glm::vec2 & to, const glm::vec4 & color = glm::vec4(1.0f), float rounding = 0.0f, uint32_t roundingCornersFlags = ImDrawCornerFlags_All);
        static void image(std::shared_ptr<Texture> pTexture, const glm::vec2 & from, const glm::vec2 & to, const glm::vec4 & color = glm::vec4(1.0f));
        static void imageRounded(std::shared_ptr<Texture> pTexture, const glm::vec2 & from, const glm::vec2 & to, const glm::vec4 & color = glm::vec4(1.0f), float rounding = 0.0f, uint32_t roundingCornersFlags = ImDrawCornerFlags_All);

    private:
        static glm::vec2 m_windowSize;
    };
}
