#pragma once

#include <glm/gtc/constants.hpp>

#include "Light.h"

class PointLight : public Light
{
public:
    PointLight(const glm::mat4 & light_to_world = glm::mat4(1.0f),
               const glm::highp_dvec3 & position       = glm::highp_dvec3(0.0f), 
               const glm::highp_dvec3 & color          = glm::highp_dvec3(1.0f),
                     double intensity            = 1.0f)
        : Light(light_to_world)
    {
        m_color     = color;
        m_intensity = intensity;

        m_position = position;
    }

    void illuminate(const glm::highp_dvec3 & hit_point, glm::highp_dvec3 & light_dir, glm::highp_dvec3 & light_intensity, double & distance) const override
    {
        light_dir =  glm::normalize(m_position - hit_point);
        
        auto length = glm::length(light_dir);

        light_intensity = m_color * m_intensity / (4.0f * glm::pi<double>() * length);
        distance = glm::distance(hit_point, m_position);
    }

    void update() override
    {
        m_position = glm::highp_dvec3(m_model_matrix * glm::vec4(m_position, 1.0f));
    }

    glm::highp_dvec3 m_position;
};
