#include "framework/window/Window.h"
#include "framework/window/Input.h"
#include "framework/utilities/DebugOutputGL.h"
#include "framework/gui/GUI.h"
#include "core_engine/Core.h"
#include "core_engine/CoreServices.h"

namespace mango
{
    GLFWwindow * Window::m_window = nullptr;
    std::string  Window::m_title = "";
    glm::vec2    Window::m_window_size = glm::vec2(0);

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

        glfwSetErrorCallback(error_callback);

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
        glViewport(0, 0, width, height);
        
        setVSync(false);
        glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);

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
        return int(m_window_size.x);
    }

    int Window::getHeight()
    {
        return  int(m_window_size.y);
    }

    glm::vec2 Window::getCenter()
    {
        return m_window_size / 2.0f;
    }

    float Window::getAspectRatio()
    {
        return (m_window_size.x / m_window_size.y);
    }

    const std::string& Window::getTitle()
    {
        return m_title;
    }

    void Window::setVSync(bool enabled)
    {
        auto value = enabled ? 1 : 0;

        glfwSwapInterval(value);
    }

    void Window::bindDefaultFramebuffer()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, m_window_size.x, m_window_size.y);
    }

    void Window::framebuffer_size_callback(GLFWwindow * window, int width, int height)
    {
        glViewport(0, 0, width, height);

        m_window_size.x = float(width);
        m_window_size.y = float(height);

        GUI::updateWindowSize(float(width), float(height));
        CoreServices::getRenderer()->resize(width, height);
    }
}
