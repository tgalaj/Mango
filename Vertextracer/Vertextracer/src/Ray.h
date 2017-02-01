#pragma once

#include <glm/glm.hpp>

class Ray
{
public:
    Ray() : Ray(glm::highp_dvec3(0.0f), glm::highp_dvec3(0, 0, -1)) {}

    Ray(const glm::highp_dvec3 & origin, const glm::highp_dvec3 & direction) 
        : m_origin(origin), 
          m_direction(direction),
          t_min(0.1f),
          t_max(1000.0f)
    {
        m_direction = glm::normalize(m_direction);
    }

    glm::highp_dvec3 m_origin;
    glm::highp_dvec3 m_direction; // normalized
    
    const double t_min, t_max;
};
