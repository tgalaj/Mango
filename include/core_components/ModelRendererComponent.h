#pragma once
#include <memory>

#include "framework/rendering/Model.h"

namespace Vertex
{
    class ModelRendererComponent
    {
    public:
        enum class RenderQueue { RQ_OPAQUE, RQ_ALPHA };

        ModelRendererComponent()
            : m_render_queue(RenderQueue::RQ_OPAQUE) 
        {}

        explicit ModelRendererComponent(const Model & model, RenderQueue render_queue = RenderQueue::RQ_OPAQUE)
            : m_model(model),
              m_render_queue(render_queue)
        {}

        RenderQueue getRenderQueue() const { return m_render_queue; }

        Model m_model;

    private:
        RenderQueue m_render_queue;
    };
}
