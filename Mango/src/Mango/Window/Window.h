#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include <iostream>
#include <string>
#include <vector>

namespace mango
{

    struct MonitorVideoMode
    {
        int32_t width;
        int32_t height;
        int32_t refreshRate;
    };

    class Window final
    {
    public:
        Window();
        ~Window();

        static void create(uint32_t width, uint32_t height, const std::string & title);
        static void endFrame();

        static int isCloseRequested();

        static int                           getWidth();
        static int                           getHeight();
        static glm::vec2                     getCenter();
        static glm::vec2                     getDpiScale();
        static float                         getAspectRatio();
        static std::vector<MonitorVideoMode> getPrimaryMonitorVideoModes();
        static GLFWwindow*                   getNativeWindow();

        static const std::string & getTitle();

        static void setVSync(bool enabled);
        static void bindDefaultFramebuffer();

        static bool isFullscreen();
        static void setFullscreen(bool fullscreen);

    private:
        static GLFWwindow  * m_window;
        static GLFWmonitor * m_monitor;
        static std::string   m_title;
        static glm::ivec2    m_windowPos;
        static glm::ivec2    m_windowSize;
        static glm::ivec2    m_viewportSize;

        static void errorCallback(int error, const char* description)
        {
            std::cerr << description << std::endl;
        }

        static void framebufferSizeCallback(GLFWwindow * window, int width, int height);
    };
}
