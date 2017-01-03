#pragma once

#include <glm/glm.hpp>

class Ray
{
public:
    Ray() : Ray(glm::vec3(0.0f), glm::vec3(0, 0, -1)) {}

    Ray(const glm::vec3 & origin, const glm::vec3 & direction) 
        : m_origin(origin), 
          m_direction(direction),
          t_min(0.1f),
          t_max(1000.0f)
    {
        m_direction = glm::normalize(m_direction);
    }

    glm::vec3 m_origin;
    glm::vec3 m_direction; // normalized
    
    const float t_min, t_max;
};
