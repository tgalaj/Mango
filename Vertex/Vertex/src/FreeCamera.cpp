#include "FreeCamera.h"
#include "Input.h"

#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\quaternion.hpp>
#include <glm\gtx\quaternion.hpp>

FreeCamera::FreeCamera(float _fieldOfView, float _viewportWidth, float _viewportHeight, float _near, float _far)
    : PerspectiveCamera(_fieldOfView, _viewportWidth, _viewportHeight, _near, _far),
      moveSpeed(10.0f),
      mouseSensitivity(0.2f),
      currMouseX(0),
      currMouseY(0),
      isFirstMouseClick(true)
{
    update();
}

void FreeCamera::update()
{
    view = glm::lookAt(position, position + direction, up);
    projection = glm::perspective(glm::radians(fieldOfView), viewportWidth / viewportHeight, nearPlane, farPlane);

    viewProjection = projection * view;

    isDirty = false;
}

void FreeCamera::processInput(float deltaTime)
{
    if (Input::getKeyDown(Input::W))
    {
        position += direction * moveSpeed * deltaTime;
        isDirty  = true;
    }
    if (Input::getKeyDown(Input::A))
    {
        position -= glm::normalize(glm::cross(direction, up)) * moveSpeed * deltaTime;
        isDirty  = true;
    }
    if (Input::getKeyDown(Input::S))
    {
        position -= direction * moveSpeed * deltaTime;
        isDirty = true;
    }
    if (Input::getKeyDown(Input::D))
    {
        position += glm::normalize(glm::cross(direction, up)) * moveSpeed * deltaTime;
        isDirty = true;
    }
    if (Input::getKeyDown(Input::E))
    {
        position -= up * moveSpeed * deltaTime;
        isDirty = true;
    }
    if (Input::getKeyDown(Input::Q))
    {
        position += up * moveSpeed * deltaTime;
        isDirty = true;
    }
    if (Input::getMouseButtonDown(Input::Mouse2))
    {
        SDL_SetRelativeMouseMode(SDL_TRUE);

        if (isFirstMouseClick)
        {
            SDL_GetRelativeMouseState(&currMouseX, &currMouseY);
            isFirstMouseClick = false;
        }

        SDL_GetRelativeMouseState(&currMouseX, &currMouseY);
        float x_offset = currMouseX;
        float y_offset = -currMouseY;

        x_offset *= mouseSensitivity;
        y_offset *= mouseSensitivity;

        /* TODO: rotations using quaternions */
        yaw   += x_offset;
        pitch += y_offset;
        
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

        direction = glm::normalize(direction);

        isDirty = true;
    }
    if (Input::getMouseButtonUp(Input::Mouse2))
    {
        SDL_SetRelativeMouseMode(SDL_FALSE);
        SDL_WarpMouseInWindow(nullptr, viewportWidth * 0.5f, viewportHeight * 0.5f);

        isFirstMouseClick = true;
    }
}

void FreeCamera::setMoveSpeed(float _moveSpeed)
{
    moveSpeed = _moveSpeed;
}

void FreeCamera::setMouseSensitivity(float _mouseSensitivity)
{
    mouseSensitivity = _mouseSensitivity;
}
