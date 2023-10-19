#include "mgpch.h"
#include "CoreEngine/CoreServices.h"

namespace mango
{
    Core            * CoreServices::m_coreEngine  = nullptr;
    RenderingSystem * CoreServices::m_renderer    = nullptr;

    Core * const CoreServices::getCore()
    {
        return m_coreEngine;
    }

    RenderingSystem * const CoreServices::getRenderer()
    {
        return m_renderer;
    }

    void CoreServices::provide(Core * core)
    {
        m_coreEngine = core;
    }

    void CoreServices::provide(RenderingSystem * renderer)
    {
        m_renderer = renderer;
    }
}
