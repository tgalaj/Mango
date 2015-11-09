#include "CoreServices.h"

Renderer         * CoreServices::renderer      = nullptr;
VertexEngineCore * CoreServices::core          = nullptr;

VertexEngineCore * const CoreServices::getCore()
{
    return core;
}

Renderer * const CoreServices::getRenderer()
{
    return renderer;
}

void CoreServices::provide(VertexEngineCore * _core)
{
    core = _core;
}

void CoreServices::provide(Renderer * _renderer)
{
    renderer = _renderer;
}
