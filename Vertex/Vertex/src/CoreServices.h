#pragma once

#include "Renderer.h"

class CoreServices final
{
    friend class VertexEngineCore;

public:
    CoreServices();
    ~CoreServices();

    static Renderer * getRenderer();

private:
    static void provide(Renderer * _renderer);
    
    static Renderer * renderer;
};

