#include "framework/window/Window.h"
#include "framework/window/Input.h"
#include "framework/utilities/DebugOutputGL.h"
#include "framework/gui/GUI.h"
#include "core_engine/Core.h"
#include "core_engine/CoreServices.h"

namespace mango
{
    GLFWwindow*  Window::m_window        = nullptr;
    GLFWmonitor* Window::m_monitor       = nullptr;
    std::string  Window::m_title         = "";
    glm::ivec2   Window::m_window_pos    = glm::ivec2(0);
    glm::ivec2   Window::m_window_size   = glm::ivec2(0);
    glm::ivec2   Window::m_viewport_size = glm::ivec2(0);

    Window::Window()
    {
    }

    Window::~Window()
    {
        glfwDestroyWindow(m_window);
        glfwTerminate();
    }

    void Window::createWindow(unsigned int width, unsigned int height, const std::string & title)
    {
        m_title = title;
        m_window_size = glm::vec2(width, height);

        if (!glfwInit())
        {
            std::cerr << "VE ERROR: Could not initialize GLFW." << std::endl;
            exit(EXIT_FAILURE);
        }

        glfwSetErrorCallback(errorCallback);

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, MIN_GL_VERSION_MAJOR);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, MIN_GL_VERSION_MINOR);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_SAMPLES, 4);

        #ifdef _DEBUG
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
        #endif

        m_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

        if (!m_window)
        {
            std::cerr << "VE ERROR: Could not create window and OpenGL context." << std::endl;

            glfwTerminate();
            exit(EXIT_FAILURE);
        }

        glfwMakeContextCurrent(m_window);

        /* Initialize GLAD */
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cerr << "VE ERROR: Could not initialize GLAD." << std::endl;
            exit(EXIT_FAILURE);
        }

        #ifdef _DEBUG
        GLint flags;
        glGetIntegerv(GL_CONTEXT_FLAGS, &flags);

        if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
        {
            /* Create OpenGL debug context */
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

            /* Register callback */
            glDebugMessageCallback(DebugOutputGL::GLerrorCallback, nullptr /*userParam*/);
            glDebugMessageControl(GL_DONT_CARE /*source*/, GL_DONT_CARE /*type*/, GL_DEBUG_SEVERITY_MEDIUM /*severity*/, 0 /*count*/, nullptr /*ids*/, GL_TRUE /*enabled*/);
        }
        #endif

        /* Set the viewport */
        glfwGetFramebufferSize(m_window, &m_viewport_size.x, &m_viewport_size.y);
        glViewport(0, 0, m_viewport_size.x, m_viewport_size.y);
        
        setVSync(false);
        glfwSetWindowSizeCallback(m_window, framebufferSizeCallback);

        m_monitor = glfwGetPrimaryMonitor();

        glfwGetWindowSize(m_window, &m_window_size.x, &m_window_size.y);
        glfwGetWindowPos(m_window, &m_window_pos.x, &m_window_pos.y);

        /* Init Input & GUI */
        Input::init(m_window);
        GUI::init(m_window);
    }

    void Window::endFrame()
    {
        glfwPollEvents();
        glfwSwapBuffers(m_window);
    }

    int Window::isCloseRequested()
    {
        return glfwWindowShouldClose(m_window);
    }

    int Window::getWidth()
    {
        return m_viewport_size.x;
    }

    int Window::getHeight()
    {
        return  m_viewport_size.y;
    }

    glm::vec2 Window::getCenter()
    {
        return glm::vec2(m_viewport_size) / 2.0f;
    }

    glm::vec2 Window::getDpiScale()
    {
        glm::vec2 dpi_scale{};

        glfwGetWindowContentScale(m_window, &dpi_scale.x, &dpi_scale.y);

        return dpi_scale;
    }

    float Window::getAspectRatio()
    {
        return float(m_viewport_size.x) / float(m_viewport_size.y);
    }

    std::vector<MonitorVideoMode> Window::getPrimaryMonitorVideoModes()
    {
        if (m_monitor == nullptr) return {};

        int modes_count;
        const GLFWvidmode* modes = glfwGetVideoModes(m_monitor, &modes_count);

        std::vector<MonitorVideoMode> video_modes(modes_count);

        for (uint32_t i = 0; i < modes_count; ++i)
        {
            video_modes[i] = { modes[i].width, modes[i].height, modes[i].refreshRate };
        }

        return video_modes;
    }

    const std::string& Window::getTitle()
    {
        return m_title;
    }

    void Window::setVSync(bool enabled)
    {
        auto value = enabled ? true : false;

        glfwSwapInterval(value);
    }

    void Window::bindDefaultFramebuffer()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, m_viewport_size.x, m_viewport_size.y);
    }

    bool Window::isFullscreen()
    {
        return glfwGetWindowMonitor(m_window) != nullptr;
    }

    void Window::setFullscreen(bool fullscreen)
    {
        if (isFullscreen() == fullscreen) return;

        if (fullscreen)
        {
            // backup window position and window size
            glfwGetWindowSize(m_window, &m_window_size.x, &m_window_size.y);
            glfwGetWindowPos(m_window, &m_window_pos.x, &m_window_pos.y);

            // get resolution of the monitor
            const GLFWvidmode* mode = glfwGetVideoMode(m_monitor);

            // switch to fullscreen
            glfwSetWindowMonitor(m_window, m_monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        }
        else
        {
            // restore last window size and position
            glfwSetWindowMonitor(m_window, nullptr, m_window_pos.x, m_window_pos.y, m_window_size.x, m_window_size.y, 0);
        }
    }

    void Window::framebufferSizeCallback(GLFWwindow* window, int width, int height)
    {
        glfwGetFramebufferSize(window, &m_viewport_size.x, &m_viewport_size.y);
        glViewport(0, 0, m_viewport_size.x, m_viewport_size.y);

        GUI::updateWindowSize(float(width), float(height));
        CoreServices::getRenderer()->resize(width, height);
    }
}
