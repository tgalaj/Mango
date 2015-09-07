#pragma once

#include "BaseGame.h"
#include "CoreServices.h"
#include "Input.h"
#include "Time.h"
#include "Window.h"
//#include "Renderer.h"

#define MIN_GL_VERSION_MAJOR 4
#define MIN_GL_VERSION_MINOR 4

class VertexEngineCore final
{
public:
    VertexEngineCore(const char * title, unsigned int width, unsigned int height, Uint32 flags = 0);
    ~VertexEngineCore();

    VertexEngineCore(const VertexEngineCore &)             = delete;
    VertexEngineCore & operator=(const VertexEngineCore &) = delete;

    static void         setClearColor(float r, float g, float b, float a);
    static void         setVSync(bool enabled);
    static void         quitApp();
    static unsigned int getFPS();

    void start(BaseGame * game);

private:
    static bool quit;

    BaseGame * game;
    Window   * window;
    Renderer * renderer;

    static unsigned int fps;
};

