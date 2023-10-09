#include "core_engine/CoreServices.h"

namespace mango
{
    Core      * CoreServices::m_core_engine  = nullptr;
    RenderingSystem * CoreServices::m_renderer     = nullptr;

    Core * const CoreServices::getCore()
    {
        return m_core_engine;
    }

    RenderingSystem * const CoreServices::getRenderer()
    {
        return m_renderer;
    }

    void CoreServices::provide(Core * core)
    {
        m_core_engine = core;
    }

    void CoreServices::provide(RenderingSystem * renderer)
    {
        m_renderer = renderer;
    }
}
