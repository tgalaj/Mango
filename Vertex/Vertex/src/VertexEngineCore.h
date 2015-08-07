#pragma once

#include "BaseGame.h"
#include "Input.h"
#include "Time.h"
#include "Window.h"

#define MIN_GL_VERSION_MAJOR 4
#define MIN_GL_VERSION_MINOR 4

class Window;

class VertexEngineCore final
{
public:
    VertexEngineCore(const char *title, unsigned int width, unsigned int height, BaseGame *game, Uint32 flags = 0);
    ~VertexEngineCore();

    VertexEngineCore(const VertexEngineCore&)            = delete;
    VertexEngineCore& operator=(const VertexEngineCore&) = delete;

    void setClearColor(float r, float g, float b, float a);
    void setVSync(bool enabled);
    void start();

private:
    BaseGame *game;
    Window   *window;

    const unsigned int TICKS_PER_SECOND;
    const unsigned int SKIP_TICKS;
    const unsigned int MAX_FRAMESKIP;
};

