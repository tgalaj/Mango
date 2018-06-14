#pragma once
#include <memory>

#include "framework/rendering/Model.h"

namespace Vertex
{
    class ModelRendererComponent
    {
    public:
        ModelRendererComponent() {}

        explicit ModelRendererComponent(const Model & model)
            : m_model(model)
        {}

        Model m_model;
    };
}
