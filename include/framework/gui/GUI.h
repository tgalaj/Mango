#pragma once

#include "imgui/imgui.h"
#include "imgui_impl_glfw_gl3.h"
#include "imgui/imgui_utils.h"

#include "framework/window/Window.h"

namespace Vertex
{
    class GUI
    {
    public:
        typedef void(*Style)(ImGuiStyle &);

        ~GUI()
        {
            ImGui_ImplGlfwGL3_Shutdown();
        }

        static void init(GLFWwindow * window)
        {
            ImGui_ImplGlfwGL3_Init(window, true);
            setStyle(GUIStyle::RayTeak, 0.95f);

            m_window_size = glm::vec2(Window::getWidth(), Window::getHeight());
        }

        static void prepare()
        {
            ImGui_ImplGlfwGL3_NewFrame();
        }

        static void render()
        {
            glViewport(0, 0, GLsizei(m_window_size.x), GLsizei(m_window_size.y));
            ImGui::Render();
        }

        static void setStyle(Style style, float alpha)
        {
            ImGui::SetupImGuiStyle(style, alpha);
        }

        static void updateWindowSize(float width, float height)
        {
            m_window_size = glm::vec2(width, height);
        }

    private:
        static glm::vec2 m_window_size;
    };
}
