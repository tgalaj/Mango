#pragma once

#include "Renderer.h"

class CoreServices final
{
    friend class VertexEngineCore;

public:
    CoreServices();
    ~CoreServices();

    static VertexEngineCore * const getCore();
    static Renderer         * const getRenderer();

private:
    static void provide(VertexEngineCore * _core);
    static void provide(Renderer         * _renderer);
    
    static VertexEngineCore * core;
    static Renderer         * renderer;
};

