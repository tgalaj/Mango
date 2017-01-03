#pragma once

#include <stdint.h>
#include "Ray.h"

class Camera
{
public:
    Camera(uint32_t width, uint32_t height, float fov);

    Ray getPrimaryRay(uint32_t x, uint32_t y);

    uint32_t m_width, m_height;
    float m_fov;
    float m_aspect_ratio;
};
