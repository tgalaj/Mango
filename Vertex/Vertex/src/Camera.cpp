#include "Camera.h"
#include <glm\gtc\matrix_transform.hpp>

Camera::Camera()
    : viewProjection(glm::mat4()),
      view          (glm::mat4()),
      projection    (glm::mat4()),
      up            (glm::vec3(0.0f, 1.0f, 0.0f)),
      position      (glm::vec3(3.0f, 3.0f, 3.0f)),
      lookAtPoint   (glm::vec3(0.0f)),
      direction     (lookAtPoint - position),
      nearPlane     (0.0f),
      farPlane      (10.0f),
      viewportWidth (480.0f),
      viewportHeight(640.0f)
{

}


Camera::~Camera()
{
}

void Camera::lookAt(glm::vec3 &point)
{
    lookAtPoint = point;
}

void Camera::setPosition(float x, float y, float z)
{
    position = glm::vec3(x, y, z);
}

void Camera::setUpVector(float x, float y, float z)
{
    up = glm::vec3(x, y, z);
}

void Camera::setNearPlane(float near)
{
    nearPlane = near;
}

void Camera::setFarPlane(float far)
{
    farPlane = far;
}

void Camera::setNearAndFarPlane(float near, float far)
{
    if(near < 0.0f || far <= 0.0f)
        return;

    if(near < far)
    {
        nearPlane = near;
        farPlane  = far;
    }
    else
    {
        nearPlane = far;
        farPlane  = near;
    }
}

void Camera::setViewport(float width, float height)
{
    viewportWidth  = width;
    viewportHeight = height;
}

glm::mat4 Camera::getViewProjection()
{
    return projection*view;
}

glm::mat4 Camera::getView()
{
    return view;
}

glm::mat4 Camera::getProjection()
{
    return projection;
}

glm::vec3 Camera::getDirection()
{
    return direction;
}

glm::vec3 Camera::getUpVector()
{
    return up;
}

glm::vec3 Camera::getPosition()
{
    return position;
}

float Camera::getNearPlane()
{
    return nearPlane;
}

float Camera::getFarPlane()
{
    return farPlane;
}

float Camera::geViewportWidth()
{
    return viewportWidth;
}

float Camera::geViewportHeight()
{
    return viewportHeight;
}