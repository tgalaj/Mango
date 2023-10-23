#include "mgpch.h"
#include "Core/CoreServices.h"

namespace mango
{
    mango::Application     * CoreServices::m_application = nullptr;
    mango::RenderingSystem * CoreServices::m_renderer    = nullptr;

    mango::Application* const CoreServices::getApplication()
    {
        return m_application;
    }

    RenderingSystem* const CoreServices::getRenderer()
    {
        return m_renderer;
    }

    void CoreServices::provide(Application* application)
    {
        m_application = application;
    }

    void CoreServices::provide(RenderingSystem * renderer)
    {
        m_renderer = renderer;
    }
}
