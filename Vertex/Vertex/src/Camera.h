#pragma once

#include <glm\glm.hpp>

class Camera
{
public:
    Camera();
    virtual ~Camera();

    virtual void lookAt(glm::vec3 point) = 0;

private:
    glm::mat4 view;
    glm::mat4 projection;

    glm::vec3 direction;
    glm::vec3 up;
    glm::vec3 position;

    float nearPlane, farPlane;
    float viewportWidth, viewportHeight;
};

