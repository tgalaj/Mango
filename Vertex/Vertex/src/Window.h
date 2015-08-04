#pragma once

#include <cstdio>
#include <string>

#include <GL/glew.h>
#include <SDL2\SDL.h>
#include <SDL2\SDL_opengl.h>

#include "VertexEngineCore.h"

class Window final
{
    friend class VertexEngineCore;

    Window(const char *title, int width, int height, Uint32 flags);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    SDL_GLContext m_glContext;
    SDL_Window   *m_sdlWindow;
    unsigned int  m_width;
    unsigned int  m_height;
    bool          m_isGood;
};

