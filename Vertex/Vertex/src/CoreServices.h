#pragma once

#include "Renderer.h"
#include "ShaderManager.h"

class CoreServices final
{
    friend class VertexEngineCore;

public:
    CoreServices()  = delete;
    ~CoreServices() = delete;

    static VertexEngineCore * const getCore();
    static Renderer         * const getRenderer();
    static ShaderManager    * const getShaderManager();

private:
    static void provide(VertexEngineCore * _core);
    static void provide(Renderer         * _renderer);
    static void provide(ShaderManager    * _shaderManager);
    
    static VertexEngineCore * core;
    static Renderer         * renderer;
    static ShaderManager    * shaderManager;
};

