#pragma once

#include <stdint.h>
#include <glm/gtc/matrix_transform.hpp>
#include "Ray.h"

class Camera
{
public:
    Camera(uint32_t width, uint32_t height, double fov, const glm::highp_dvec3 & eye, const glm::highp_dvec3 & look_at, const glm::highp_dvec3 & up);
    
    Ray getPrimaryRay(double x, double y);
    void update();

    uint32_t m_width, m_height;
    glm::highp_dvec3 m_eye;
    double m_fov;
    double m_aspect_ratio;

private:
    glm::mat4 m_cam_transform;
    double m_angle;
};
