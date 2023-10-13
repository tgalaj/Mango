#pragma once
#include "PointLightComponent.h"

namespace mango
{
    class SpotLightComponent : public PointLightComponent
    {
    public:
        SpotLightComponent()
            : m_cutoff(0.3f)
        {
            setShadowInfo(ShadowInfo(glm::perspective(glm::acos(m_cutoff), 1.0f, 0.001f, 100.0f), true));
        }

        SpotLightComponent(const glm::vec3 & color, float intensity, const Attenuation & attenuation, float cutoff)
            : PointLightComponent(color, intensity, attenuation),
              m_cutoff(cutoff)
        {
            setShadowInfo(ShadowInfo(glm::perspective(glm::acos(m_cutoff), 1.0f, 0.001f, 100.0f), true));
        }

        /* 
         * Set angle in degrees. 
         */
        void setCutOffAngle(float angle)
        {
            m_cutoff = glm::cos(glm::radians(angle));
        }

        /* 
         * Returns cosine of cutoff angle. 
         */
        float getCutOffAngle() const { return m_cutoff; }

    private:
        float m_cutoff;
    };
}
