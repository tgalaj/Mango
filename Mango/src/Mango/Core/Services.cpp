#include "mgpch.h"
#include "Services.h"
#include "Mango/Systems/RenderingSystem.h"

namespace mango
{
    Application     * Services::m_application  = nullptr;
    RenderingSystem * Services::m_renderer     = nullptr;
    SceneManager    * Services::m_sceneManager = nullptr;
    EventBus        * Services::m_eventBus     = nullptr;

    Application* const Services::application()
    {
        return m_application;
    }

    RenderingSystem* const Services::renderer()
    {
        return m_renderer;
    }

    SceneManager* const Services::sceneManager()
    {
        return m_sceneManager;
    }

    EventBus* const Services::eventBus()
    {
        return m_eventBus;
    }

    void Services::provide(Application* application)
    {
        m_application = application;
    }

    void Services::provide(RenderingSystem * renderer)
    {
        m_renderer = renderer;
    }

    void Services::provide(SceneManager* sceneManager)
    {
        m_sceneManager = sceneManager;
    }

    void Services::provide(EventBus* eventBus)
    {
        m_eventBus = eventBus;
    }

}
