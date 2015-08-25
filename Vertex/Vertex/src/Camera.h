#pragma once

#include <glm\glm.hpp>

class Camera
{
public:
    Camera();
    virtual ~Camera();

    virtual void update() = 0; //TODO: think about solution that doesn't require calling this method

    /* Setters */
    virtual void lookAt            (glm::vec3 &point);
    virtual void setPosition       (float x, float y, float z);
    virtual void setUpVector       (float x, float y, float z);
    virtual void setNearPlane      (float near);
    virtual void setFarPlane       (float far);
    virtual void setNearAndFarPlane(float near, float far);
    virtual void setViewport       (float width, float height);

    /* Getters */
    virtual glm::mat4 getViewProjection();
    virtual glm::mat4 getView();
    virtual glm::mat4 getProjection();
                      
    virtual glm::vec3 getDirection();
    virtual glm::vec3 getUpVector();
    virtual glm::vec3 getPosition();

    virtual float getNearPlane();
    virtual float getFarPlane();

    virtual float geViewportWidth();
    virtual float geViewportHeight();

protected:
    glm::mat4 viewProjection;
    glm::mat4 view;
    glm::mat4 projection;

    glm::vec3 direction;
    glm::vec3 up;
    glm::vec3 position;
    glm::vec3 lookAtPoint;

    float nearPlane, farPlane;
    float viewportWidth, viewportHeight;
};

