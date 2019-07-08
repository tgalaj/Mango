#pragma once

#include <glm/gtx/component_wise.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "BaseLightComponent.h"
#include "framework/rendering/Attenuation.h"


namespace Vertex
{
    class PointLightComponent : public BaseLightComponent
    {
    public:
        PointLightComponent()
            : BaseLightComponent    (glm::vec3(1.0f), 1.0f),
              m_attenuation(0.0, 0.0f, 1.0f)
        {
            calculateRange();
            setShadowInfo(ShadowInfo(glm::perspective(glm::radians(90.0f), 1.0f, 0.001f, 100.0f), true));
        }

        PointLightComponent(const glm::vec3 &   color, 
                            float intensity, 
                            const Attenuation & attenuation)
            : BaseLightComponent    (color, intensity),
              m_attenuation(attenuation)
        {
            calculateRange();
            setShadowInfo(ShadowInfo(glm::perspective(glm::radians(90.0f), 1.0f, 0.001f, 100.0f), true));
        }

        void setAttenuation(float constant, float linear, float quadratic)
        {
            m_attenuation = Attenuation(constant, linear, quadratic);
            calculateRange();
        }

        Attenuation m_attenuation;
        float m_range;

    private:
        static const int COLOR_DEPTH = 256;

        void calculateRange()
        {
            float a = m_attenuation.m_quadratic;
            float b = m_attenuation.m_linear;
            float c = m_attenuation.m_constant - COLOR_DEPTH * m_intensity * glm::compMax(m_color);

            m_range = (-b + glm::sqrt(b * b - 4 * a * c)) / (2 * a);
        }
    };
}
