#pragma once

#include <glm\glm.hpp>

class Camera
{
public:
    friend class Scene;

    Camera();
    virtual ~Camera();

    virtual void update() = 0;
    virtual void processInput(float deltaTime) {};

    /* Setters */
    void lookAt            (glm::vec3 & point);
    void setPosition       (float x, float y, float z);
    void setDirection      (float x, float y, float z);
    void setUpVector       (float x, float y, float z);
    void setNearPlane      (float near);
    void setFarPlane       (float far);
    void setNearAndFarPlane(float near, float far);
    void setViewport       (float width, float height);
    void setFieldOfView    (float fov);
    void setZoom           (float zoom);

    /* Getters */
    glm::mat4 getViewProjection();
    glm::mat4 getView();
    glm::mat4 getProjection();
              
    glm::vec3 getDirection();
    glm::vec3 getUpVector();
    glm::vec3 getPosition();

    float getNearPlane();
    float getFarPlane();

    float geViewportWidth();
    float geViewportHeight();

    float getFieldOfView();
    float getZoom();

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
    float fieldOfView, zoom;

    bool isDirty;
};

