#pragma once

#include "BaseGame.h"
#include "Input.h"
#include "Time.h"
#include "Window.h"

#define MIN_GL_VERSION_MAJOR 4
#define MIN_GL_VERSION_MINOR 4

class VertexEngineCore final
{
public:
    VertexEngineCore(const char * title, unsigned int width, unsigned int height, Uint32 flags = 0);
    ~VertexEngineCore();

    VertexEngineCore(const VertexEngineCore &)             = delete;
    VertexEngineCore & operator=(const VertexEngineCore &) = delete;

    static void setClearColor(float r, float g, float b, float a);
    static void setVSync(bool enabled);
    static void quitApp();

    void start(BaseGame * game);

private:
    static bool quit;

    BaseGame * game;
    Window   * window;

    const unsigned int TICKS_PER_SECOND;
    const unsigned int SKIP_TICKS;
    const unsigned int MAX_FRAMESKIP;
};

