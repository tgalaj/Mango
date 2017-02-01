#pragma once
#include "Light.h"

class DirectionalLight : public Light
{
public:
    DirectionalLight(const glm::mat4 & light_to_world = glm::mat4(1.0f), 
                     const glm::highp_dvec3 & direction      = glm::highp_dvec3(1.0f, 1.0f, -1.0f),
                     const glm::highp_dvec3 & color          = glm::highp_dvec3(1.0f),
                           double intensity            = 1.0f)
        : Light(light_to_world)
    {
        m_color     = color;
        m_intensity = intensity;

        m_direction = direction;
    }

    void illuminate(const glm::highp_dvec3 & hit_point, glm::highp_dvec3 & light_dir, glm::highp_dvec3 & light_intensity, double & distance) const override
    {
        light_dir       = m_direction;
        light_intensity = m_intensity * m_color;
        distance        = std::numeric_limits<double>::max();
    }

    void update() override
    {
        m_direction = glm::highp_dvec3(m_model_matrix * glm::vec4(m_direction, 0.0f));
        m_direction = glm::normalize(m_direction);
    }


    glm::highp_dvec3 m_direction;
};
