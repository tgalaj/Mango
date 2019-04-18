#pragma once

#include "BaseLightComponent.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Vertex
{
    class DirectionalLightComponent : public BaseLightComponent
    {
    public:
        DirectionalLightComponent(const glm::vec3 & color, float intensity, float size = 20.0f)
            : BaseLightComponent(color, intensity)
        {
            setShadowInfo(ShadowInfo(glm::ortho(-size, size, -size, size, -size, size), true));
        }

        explicit DirectionalLightComponent()
            : BaseLightComponent(glm::vec3(1.0f), 1.0f)
        {}
    };
}

