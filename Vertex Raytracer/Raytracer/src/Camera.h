#pragma once

#include <glm/glm.hpp>

#include "Ray.h"

class Camera
{
public:
    Camera(size_t width, size_t height);
    ~Camera();

    void update();

    Ray rayFromCamera(int i, int j) const;

    glm::vec3 eye;
    glm::vec3 lookAt;
    glm::vec3 up;

    glm::vec3 n;
    glm::vec3 u;
    glm::vec3 v;

    float aspect;
    float fovy;

    size_t img_width;
    size_t img_height;

private:
    float tanFovy;
    float tanFovx;
};

