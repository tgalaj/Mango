#pragma once

#include <glm/gtc/matrix_transform.hpp>

#include "BaseLightComponent.h"

namespace Vertex
{
    class DirectionalLightComponent : public BaseLightComponent
    {
    public:
        DirectionalLightComponent(const glm::vec3 & color, float intensity)
            : BaseLightComponent(color, intensity)
        {
            float size = 20.0f;
            setShadowInfo(ShadowInfo(glm::ortho(-size, size, -size, size, -size, size), true));
        }

        explicit DirectionalLightComponent()
            : BaseLightComponent(glm::vec3(1.0f), 1.0f)
        {}
    };
}

