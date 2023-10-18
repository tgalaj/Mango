#pragma once
#include "Core.h"
#include "CoreSystems/RenderingSystem.h"

namespace mango
{
    class CoreServices final
    {
    public:
        CoreServices() = delete;
        ~CoreServices() = delete;

        static Core * const getCore();
        static RenderingSystem * const getRenderer();

    private:
        static void provide(Core * core);
        static void provide(RenderingSystem * renderer);

        static Core      * m_core_engine;
        static RenderingSystem * m_renderer;

        friend class Core;
    };
}

