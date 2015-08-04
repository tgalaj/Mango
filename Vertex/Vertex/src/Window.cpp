#include "Window.h"

Window::Window     (const char* title, int width, int height, Uint32 flags = 0) 
      : m_width    (width), 
        m_height   (height),
        m_glContext(nullptr),
        m_sdlWindow(nullptr)
{
    m_isGood = true;

    /* Initialize the SDL library */
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "SDL could not initialize! SDL error :%s\n", SDL_GetError());
        m_isGood = false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, MIN_GL_VERSION_MAJOR);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, MIN_GL_VERSION_MINOR);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);

    /* Create a windowed mode window and its OpenGL context */
    window = SDL_CreateWindow("Hello Vertex Engine!", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    if (!window)
    {
        fprintf(stderr, "Window could not be created! SDL error : %s\n", SDL_GetError());

        system("pause");
        return -1;
    }

    /* Create GL context */
    glContext = SDL_GL_CreateContext(window);

    if(!glContext)
    {
        fprintf(stderr, "OpenGL context could not be created! SDL error: %s\n", SDL_GetError());

        std::string glVersion = std::to_string(GL_VERSION_MAJOR) + "." + std::to_string(GL_VERSION_MINOR);
        fprintf(stderr, "Window creation failed! OpenGL %s not supported! Please consider updating your graphics card drivers.\n", glVersion.c_str());

        system("pause");
        return -1;
    }
}

Window::~Window()
{
    SDL_DestroyWindow(m_sdlWindow);
    m_sdlWindow = nullptr;

    /* Quit SDL subsystems */
    SDL_Quit();
}

