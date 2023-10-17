#pragma once
#include <memory>

#include "rendering/Model.h"

namespace mango
{
    class ModelRendererComponent
    {
    public:
        enum class RenderQueue { RQ_OPAQUE, RQ_ALPHA, RQ_ENVIRO_MAPPING_STATIC, RQ_ENVIRO_MAPPING_DYNAMIC };

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
