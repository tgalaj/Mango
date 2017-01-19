#pragma once

#include <glm/gtc/constants.hpp>

#include "Light.h"

class PointLight : public Light
{
public:
    PointLight(const glm::mat4 & light_to_world = glm::mat4(1.0f),
               const glm::vec3 & position       = glm::vec3(0.0f), 
               const glm::vec3 & color          = glm::vec3(1.0f),
                     float intensity            = 1.0f)
        : Light(light_to_world)
    {
        m_color     = color;
        m_intensity = intensity;

        m_position = position;
        m_position = glm::vec3(m_model_matrix * glm::vec4(m_position, 1.0f));
    }

    void illuminate(const glm::vec3 & hit_point, glm::vec3 & light_dir, glm::vec3 & light_intensity, float & distance) const override
    {
        light_dir =  glm::normalize(m_position - hit_point);
        
        auto length = glm::length(light_dir);

        light_intensity = m_color * m_intensity / (4.0f * glm::pi<float>() * length);
        distance = glm::distance(hit_point, m_position);
    }

    glm::vec3 m_position;
};
