#include "CoreServices.h"

Renderer * CoreServices::renderer = nullptr;

CoreServices::CoreServices()
{
}


CoreServices::~CoreServices()
{
}

Renderer * CoreServices::getRenderer()
{
    return renderer;
}

void CoreServices::provide(Renderer * _renderer)
{
    renderer = _renderer;
}
