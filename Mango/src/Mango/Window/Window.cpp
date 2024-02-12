#include "mgpch.h"

#include "Window.h"
#include "Input.h"
#include "Mango/Core/Services.h"
#include "Mango/Events/GamepadEvents.h"
#include "Mango/Rendering/DebugOutputGL.h"
#include "Mango/Systems/ImGuiSystem.h"
#include "Mango/Systems/RenderingSystem.h"

#define MIN_GL_VERSION_MAJOR 4
#define MIN_GL_VERSION_MINOR 6

namespace mango
{
    Window::Window(uint32_t width, uint32_t height, const std::string& title, bool maximized)
        : m_window      (nullptr),
          m_monitor     (nullptr),
          m_windowPos   (glm::ivec2(0)),
          m_windowSize  (glm::ivec2(0)),
          m_viewportSize(glm::ivec2(0))
    {
        create(width, height, title, maximized);
    }

    Window::~Window()
    {
        MG_PROFILE_ZONE_SCOPED;
        glfwDestroyWindow(m_window);
        glfwTerminate();
    }

    void Window::create(uint32_t width, uint32_t height, const std::string & title, bool maximized)
    {
        MG_PROFILE_ZONE_SCOPED;

        m_title       = title;
        m_windowSize = glm::vec2(width, height);

        if (!glfwInit())
        {
            MG_CORE_CRITICAL("Could not initialize GLFW.");
            exit(EXIT_FAILURE);
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, MIN_GL_VERSION_MAJOR);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, MIN_GL_VERSION_MINOR);
        glfwWindowHint(GLFW_OPENGL_PROFILE,        GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_SAMPLES,               4);
        glfwWindowHint(GLFW_MAXIMIZED,             maximized);

        #ifdef _DEBUG
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT,  GL_TRUE);
        #endif

        m_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

        if (!m_window)
        {
            MG_CORE_CRITICAL("Could not create window and OpenGL context.");

            glfwTerminate();
            exit(EXIT_FAILURE);
        }

        glfwMakeContextCurrent(m_window);

        /* Initialize GLAD */
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            MG_CORE_CRITICAL("Could not initialize GLAD.");
            exit(EXIT_FAILURE);
        }
        
        MG_PROFILE_GL_CONTEXT;

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
        glfwGetFramebufferSize(m_window, &m_viewportSize.x, &m_viewportSize.y);
        glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);
        
        glfwSetWindowUserPointer(m_window, this);

        glfwSetWindowSizeCallback(m_window, [](GLFWwindow* window, int width, int height)
        {
            if (width <= 0 || height <= 0)
            {
                return;
            }

            Window& data = *(Window*)glfwGetWindowUserPointer(window);

            data.m_windowSize.x = width;
            data.m_windowSize.y = height;

            glfwGetFramebufferSize(window, &data.m_viewportSize.x, &data.m_viewportSize.y);
            glViewport(0, 0, data.m_viewportSize.x, data.m_viewportSize.y);

            MG_CORE_ASSERT_MSG(Services::application()                   != nullptr, "application can't be nullptr!");
            MG_CORE_ASSERT_MSG(Services::application()->getImGuiSystem() != nullptr, "imGuiSystem can't be nullptr!");
            Services::application()->getImGuiSystem()->updateWindowSize(float(width), float(height));

            if (Services::renderer())
            {
                Services::renderer()->resize(width, height);
            }
        });

        glfwSetErrorCallback([](int error, const char* description)
        {
            MG_CORE_ERROR("GLFW error: {}", description);
        });

        glfwSetJoystickCallback([](int jid, int event)
        {
            Services::eventBus()->emit(GamepadConnectedEvent(GamepadID(jid), event == GLFW_CONNECTED));
        });

        m_monitor = glfwGetPrimaryMonitor();

        glfwGetWindowSize(m_window, &m_windowSize.x, &m_windowSize.y);
        glfwGetWindowPos(m_window, &m_windowPos.x, &m_windowPos.y);
        setVSync(false);
    }

    void Window::endFrame()
    {
        glfwPollEvents();
        glfwSwapBuffers(m_window);
        MG_PROFILE_GL_COLLECT;
    }

    int Window::isCloseRequested()
    {
        return glfwWindowShouldClose(m_window);
    }

    int Window::getWidth()
    {
        MG_PROFILE_ZONE_SCOPED;
        return m_viewportSize.x;
    }

    int Window::getHeight()
    {
        MG_PROFILE_ZONE_SCOPED;
        return  m_viewportSize.y;
    }

    glm::vec2 Window::getCenter()
    {
        MG_PROFILE_ZONE_SCOPED;
        return glm::vec2(m_viewportSize) / 2.0f;
    }

    glm::vec2 Window::getDpiScale()
    {
        MG_PROFILE_ZONE_SCOPED;
        glm::vec2 dpiScale{};

        glfwGetWindowContentScale(m_window, &dpiScale.x, &dpiScale.y);

        return dpiScale;
    }

    float Window::getAspectRatio()
    {
        MG_PROFILE_ZONE_SCOPED;
        return float(m_viewportSize.x) / float(m_viewportSize.y);
    }

    std::vector<MonitorVideoMode> Window::getPrimaryMonitorVideoModes()
    {
        MG_PROFILE_ZONE_SCOPED;
        if (m_monitor == nullptr) return {};

        int modesCount;
        const GLFWvidmode* modes = glfwGetVideoModes(m_monitor, &modesCount);

        std::vector<MonitorVideoMode> videoModes(modesCount);

        for (uint32_t i = 0; i < modesCount; ++i)
        {
            videoModes[i] = { modes[i].width, modes[i].height, modes[i].refreshRate };
        }

        return videoModes;
    }

    GLFWwindow* Window::getNativeWindow()
    {
        return m_window;
    }

    const std::string& Window::getTitle()
    {
        return m_title;
    }

    void Window::setVSync(bool enabled)
    {
        int value = enabled ? 1 : 0;

        glfwSwapInterval(value);
    }

    void Window::bindDefaultFramebuffer()
    {
        MG_PROFILE_GL_ZONE("Window::bindDefaultFramebuffer");
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, m_viewportSize.x, m_viewportSize.y);
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
            // Backup window position and window size.
            glfwGetWindowSize(m_window, &m_windowSize.x, &m_windowSize.y);
            glfwGetWindowPos(m_window, &m_windowPos.x, &m_windowPos.y);

            // Get resolution of the monitor.
            const GLFWvidmode* mode = glfwGetVideoMode(m_monitor);

            // Switch to fullscreen.
            glfwSetWindowMonitor(m_window, m_monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        }
        else
        {
            // Restore last window size and position.
            glfwSetWindowMonitor(m_window, nullptr, m_windowPos.x, m_windowPos.y, m_windowSize.x, m_windowSize.y, 0);
        }
    }
}
