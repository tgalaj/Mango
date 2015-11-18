#pragma once

#include "PerspectiveCamera.h"

class FreeCamera final : public PerspectiveCamera
{
public:
    FreeCamera(float _fieldOfView, float _viewportWidth, float _viewportHeight, float _near, float _far);
    virtual ~FreeCamera() {};

    void update() override;
    void processInput(float deltaTime) override;

    void setMoveSpeed(float _moveSpeed);
    void setMouseSensitivity(float _mouseSensitivity);

private:
    float moveSpeed;
    float mouseSensitivity;

    float yaw = -90.0f;
    float pitch = 0.0f;

    int currMouseX;
    int currMouseY;

    bool isFirstMouseClick;
};