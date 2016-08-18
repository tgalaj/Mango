#pragma once

#include "BaseGame.h"
#include "CoreAssetManager.h"
#include "CoreServices.h"
#include "Input.h"
#include "Scene.h"
#include "Time.h"
#include "ShaderManager.h"
#include "Window.h"

#define MIN_GL_VERSION_MAJOR 4
#define MIN_GL_VERSION_MINOR 5

class VertexEngineCore final
{
public:
    VertexEngineCore(const char * title, unsigned int width, unsigned int height, Uint32 flags = 0);
    ~VertexEngineCore();

    VertexEngineCore(const VertexEngineCore &)             = delete;
    VertexEngineCore & operator=(const VertexEngineCore &) = delete;

    void         setVSync(bool enabled);
    void         setScene(Scene * scene);
    void         quitApp();
    unsigned int getFPS();

    void start(BaseGame * game);

private:
    BaseGame         * game;
    CoreAssetManager * assetManager;
    Renderer         * renderer;
    Scene            * scene;
    ShaderManager    * shaderManager;
    Window           * window;

    unsigned int fps, fpsToReturn;
    bool quit;
};

