#include "CoreServices.h"

Renderer         * CoreServices::renderer      = nullptr;
ShaderManager    * CoreServices::shaderManager = nullptr;
VertexEngineCore * CoreServices::core          = nullptr;

VertexEngineCore * const CoreServices::getCore()
{
    return core;
}

Renderer * const CoreServices::getRenderer()
{
    return renderer;
}

ShaderManager * const CoreServices::getShaderManager()
{
    return shaderManager;
}

void CoreServices::provide(VertexEngineCore * _core)
{
    core = _core;
}

void CoreServices::provide(Renderer * _renderer)
{
    renderer = _renderer;
}

void CoreServices::provide(ShaderManager * _shaderManager)
{
    shaderManager = _shaderManager;
}
