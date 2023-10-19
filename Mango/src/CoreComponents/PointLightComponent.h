#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/component_wise.hpp>

#include "BaseLightComponent.h"
#include "Rendering/Attenuation.h"


namespace mango
{
    class PointLightComponent : public BaseLightComponent
    {
    public:
        PointLightComponent()
            : BaseLightComponent(glm::vec3(1.0f), 1.0f),
              attenuation       (0.0, 0.0f, 1.0f)
        {
            calculateRange();
            setShadowInfo(ShadowInfo(glm::perspective(glm::radians(90.0f), 1.0f, 0.001f, 100.0f), true));
        }

        PointLightComponent(const glm::vec3 &   color, 
                                  float         intensity, 
                            const Attenuation & attenuation)
            : BaseLightComponent(color, intensity),
              attenuation       (attenuation)
        {
            calculateRange();
            setShadowInfo(ShadowInfo(glm::perspective(glm::radians(90.0f), 1.0f, 0.001f, 100.0f), true));
        }

        void setAttenuation(float constant, float linear, float quadratic)
        {
            attenuation = Attenuation(constant, linear, quadratic);
            calculateRange();
        }

        Attenuation attenuation;
        float       range{};

    private:
        static const int COLOR_DEPTH = 255;

        void calculateRange()
        {
            float a = attenuation.quadratic;
            float b = attenuation.linear;
            float c = attenuation.constant - COLOR_DEPTH * intensity * glm::compMax(color);

            range = (-b + glm::sqrt(b * b - 4 * a * c)) / (2 * a);
        }
    };
}
