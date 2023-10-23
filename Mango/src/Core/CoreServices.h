#pragma once
#include "Application.h"
#include "Systems/RenderingSystem.h"

namespace mango
{
    class CoreServices final
    {
    public:
        CoreServices() = delete;
        ~CoreServices() = delete;

        static Application     * const getApplication();
        static RenderingSystem * const getRenderer();

    private:
        static void provide(Application     * application);
        static void provide(RenderingSystem * renderer);
    
    private:
        static Application     * m_application;
        static RenderingSystem * m_renderer;
    
    private:
        friend class Application;
    };
}

