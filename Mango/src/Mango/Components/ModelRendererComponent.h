#pragma once

#include <memory>
#include "Mango/Rendering/Model.h"

namespace mango
{
    class ModelRendererComponent
    {
    public:
        enum class RenderQueue { RQ_OPAQUE, RQ_ALPHA, RQ_ENVIRO_MAPPING_STATIC, RQ_ENVIRO_MAPPING_DYNAMIC };

        ModelRendererComponent()
            : m_renderQueue(RenderQueue::RQ_OPAQUE) 
        {}

        explicit ModelRendererComponent(const Model & model, RenderQueue renderQueue = RenderQueue::RQ_OPAQUE)
            : model        (model),
              m_renderQueue(renderQueue)
        {}

        RenderQueue getRenderQueue() const { return m_renderQueue; }

        Model model;

    private:
        RenderQueue m_renderQueue;
    };
}
