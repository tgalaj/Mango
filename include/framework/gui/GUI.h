#pragma once

#include <imgui.h>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "framework/window/Window.h"

namespace Vertex
{
    class GUI
    {
    public:
        typedef void(*Style)(ImGuiStyle &);

        ~GUI()
        {
            ImGui_ImplOpenGL3_Shutdown();
            ImGui_ImplGlfw_Shutdown();
            ImGui::DestroyContext();
        }

        static void init(GLFWwindow * window)
        {
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();

            ImGui_ImplGlfw_InitForOpenGL(window, true);
            ImGui_ImplOpenGL3_Init("#version 450");

            m_window_size = glm::vec2(Window::getWidth(), Window::getHeight());
        }

        static void prepare()
        {
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
        }

        static void render()
        {
            glViewport(0, 0, GLsizei(m_window_size.x), GLsizei(m_window_size.y));
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        static void updateWindowSize(float width, float height)
        {
            m_window_size = glm::vec2(width, height);
        }

    private:
        static glm::vec2 m_window_size;
    };
}
