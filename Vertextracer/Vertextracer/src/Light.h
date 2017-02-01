#pragma once
#include <glm/glm.hpp>

class Light
{
public:
    explicit Light(const glm::mat4 & light_to_world = glm::mat4(1.0f)) 
        : m_model_matrix(light_to_world),
          m_intensity(1.0f)
    {
    }

    virtual ~Light() {};

    virtual void illuminate(const glm::highp_dvec3 & hit_point, glm::highp_dvec3 & light_dir, glm::highp_dvec3 & light_intensity, double & distance) const = 0;
    virtual void update() = 0;

    glm::mat4 m_model_matrix; //Light to world position/direction
    glm::highp_dvec3 m_color;
    double m_intensity;

    static const glm::highp_dvec3 AMBIENT;
};
