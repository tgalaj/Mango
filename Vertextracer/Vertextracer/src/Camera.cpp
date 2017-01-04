#include "Camera.h"

Camera::Camera(uint32_t width, uint32_t height, const glm::vec3 & eye, float fov)
    : m_width(width),
      m_height(height),
      m_eye(eye),
      m_fov(fov),
      m_aspect_ratio(float(width) / float(height))
{
    update();

    m_cam_transform = glm::rotate(glm::mat4(), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));//glm::mat4(1.0f);
    glm::vec4 temp_eye = m_cam_transform * glm::vec4(m_eye, 1.0f);
    m_eye = glm::vec3(temp_eye) / temp_eye.w;
}

Ray Camera::getPrimaryRay(uint32_t x, uint32_t y)
{
    Ray primary_ray;

    float pixel_cam_x = (2.0f * ((x + 0.5f) / m_width) - 1.0f) * m_aspect_ratio * m_angle;
    float pixel_cam_y = (1.0f - 2.0f * ((y + 0.5f) / m_height)) * m_angle;

    primary_ray.m_origin    = m_eye;
    primary_ray.m_direction = glm::vec3(pixel_cam_x, pixel_cam_y, -1.0f);
    primary_ray.m_direction = glm::vec3(m_cam_transform * glm::vec4(primary_ray.m_direction, 0.0f));
    primary_ray.m_direction = glm::normalize(primary_ray.m_direction);

    return primary_ray;
}

void Camera::update()
{
    m_angle = glm::tan(glm::radians(m_fov / 2.0f));
}
