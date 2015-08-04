#pragma once

#include "Window.h"

#define MIN_GL_VERSION_MAJOR 4
#define MIN_GL_VERSION_MINOR 4

class Window;

class VertexEngineCore final
{
public:
    VertexEngineCore(const char *title, unsigned int width, unsigned int height);
    ~VertexEngineCore();

    VertexEngineCore(const VertexEngineCore&) = delete;
    VertexEngineCore& operator=(const VertexEngineCore&) = delete;

    void setVSync(bool enabled);
    void start();

private:
    Window *window;
};

