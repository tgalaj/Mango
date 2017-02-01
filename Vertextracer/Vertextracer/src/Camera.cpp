#include "Camera.h"

Camera::Camera(uint32_t width, uint32_t height, double fov, const glm::highp_dvec3 & eye, const glm::highp_dvec3 & look_at, const glm::highp_dvec3 & up)
    : m_width(width),
      m_height(height),
      m_eye(eye),
      m_fov(fov),
      m_aspect_ratio(double(width) / double(height))
{
    update();

    m_cam_transform = glm::lookAt(m_eye, look_at, up);
}

Ray Camera::getPrimaryRay(double x, double y)
{
    Ray primary_ray;

    double pixel_cam_x = (2.0f * ((x) / m_width) - 1.0f) * m_aspect_ratio * m_angle;
    double pixel_cam_y = (1.0f - 2.0f * ((y) / m_height)) * m_angle;

    primary_ray.m_origin    = m_eye;
    primary_ray.m_direction = glm::highp_dvec3(pixel_cam_x, pixel_cam_y, -1.0f);
    primary_ray.m_direction = glm::highp_dvec3(m_cam_transform * glm::vec4(primary_ray.m_direction, 0.0f));
    primary_ray.m_direction = glm::normalize(primary_ray.m_direction);

    return primary_ray;
}

void Camera::update()
{
    m_angle = glm::tan(glm::radians(m_fov / 2.0f));
}
