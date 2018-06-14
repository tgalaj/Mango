#pragma once
#include "VertexCore.h"
#include "core_systems/RenderingSystem.h"

namespace Vertex
{
    class CoreServices final
    {
    public:
        CoreServices() = delete;
        ~CoreServices() = delete;

        static VertexCore * const getCore();
        static RenderingSystem * const getRenderer();

    private:
        static void provide(VertexCore * core);
        static void provide(RenderingSystem * renderer);

        static VertexCore      * m_core_engine;
        static RenderingSystem * m_renderer;

        friend class VertexCore;
    };
}

