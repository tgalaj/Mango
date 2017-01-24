#pragma once
#include "Light.h"

class DirectionalLight : public Light
{
public:
    DirectionalLight(const glm::mat4 & light_to_world = glm::mat4(1.0f), 
                     const glm::vec3 & direction      = glm::vec3(1.0f, 1.0f, -1.0f),
                     const glm::vec3 & color          = glm::vec3(1.0f),
                           float intensity            = 1.0f)
        : Light(light_to_world)
    {
        m_color     = color;
        m_intensity = intensity;

        m_direction = direction;
    }

    void illuminate(const glm::vec3 & hit_point, glm::vec3 & light_dir, glm::vec3 & light_intensity, float & distance) const override
    {
        light_dir       = m_direction;
        light_intensity = m_intensity * m_color;
        distance        = std::numeric_limits<float>::max();
    }

    void update() override
    {
        m_direction = glm::vec3(m_model_matrix * glm::vec4(m_direction, 0.0f));
        m_direction = glm::normalize(m_direction);
    }


    glm::vec3 m_direction;
};
