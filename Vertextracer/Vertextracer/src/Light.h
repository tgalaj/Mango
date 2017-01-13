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

    virtual void illuminate(const glm::vec3 & hit_point, glm::vec3 & light_dir, glm::vec3 & light_intensity, float & distance) const = 0;

    glm::mat4 m_model_matrix; //Light to world position/direction
    glm::vec3 m_color;
    float m_intensity;
};
