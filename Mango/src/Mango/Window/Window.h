#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
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
        Window(uint32_t width, uint32_t height, const std::string& title, bool maximized = false);
        ~Window();

        int  isCloseRequested();
        void endFrame();

        int                           getWidth();
        int                           getHeight();
        glm::vec2                     getCenter();
        glm::vec2                     getDpiScale();
        float                         getAspectRatio();
        std::vector<MonitorVideoMode> getPrimaryMonitorVideoModes();
        GLFWwindow*                   getNativeWindow();

        const std::string & getTitle();

        void setVSync(bool enabled);
        void bindDefaultFramebuffer();

        bool isFullscreen();
        void setFullscreen(bool fullscreen);

    private:
        void create(uint32_t width, uint32_t height, const std::string & title, bool maximized = false);

    private:
        GLFWwindow  * m_window;
        GLFWmonitor * m_monitor;
        std::string   m_title;
        glm::ivec2    m_windowPos;
        glm::ivec2    m_windowSize;
        glm::ivec2    m_viewportSize;
    };
}
