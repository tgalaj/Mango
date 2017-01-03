#include "Camera.h"

Camera::Camera(uint32_t width, uint32_t height, float fov)
    : m_width(width),
      m_height(height),
      m_fov(fov),
      m_aspect_ratio(float(width) / float(height))
{
}
