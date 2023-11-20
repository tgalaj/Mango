#pragma once
#include "Application.h"

namespace mango
{
    class RenderingSystem;

    class Services final
    {
    public:
        Services() = delete;
        ~Services() = delete;

        static Application     * const application();
        static RenderingSystem * const renderer();
        static SceneManager    * const sceneManager();
        static EventBus        * const eventBus();

    private:
        static void provide(Application     * application);
        static void provide(RenderingSystem * renderer);
        static void provide(SceneManager    * sceneManager);
        static void provide(EventBus        * eventBus);
    
    private:
        static Application     * m_application;
        static RenderingSystem * m_renderer;
        static SceneManager    * m_sceneManager;
        static EventBus        * m_eventBus;
    
    private:
        friend class Application;
    };
}

