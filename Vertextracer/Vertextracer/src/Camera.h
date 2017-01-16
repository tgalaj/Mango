#pragma once

#include <stdint.h>
#include <glm/gtc/matrix_transform.hpp>
#include "Ray.h"

class Camera
{
public:
    Camera(uint32_t width, uint32_t height, float fov, const glm::vec3 & eye, const glm::vec3 & look_at, const glm::vec3 & up);
    
    Ray getPrimaryRay(uint32_t x, uint32_t y);
    void update();

    uint32_t m_width, m_height;
    glm::vec3 m_eye;
    float m_fov;
    float m_aspect_ratio;

private:
    glm::mat4 m_cam_transform;
    float m_angle;
};
