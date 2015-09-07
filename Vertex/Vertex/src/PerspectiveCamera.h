#pragma once
#include "Camera.h"
class PerspectiveCamera final : public Camera
{
public:
    PerspectiveCamera(float _fieldOfView, float _viewportWidth, float _viewportHeight, float _near, float _far);
    ~PerspectiveCamera();

    void update() override;

    void setFieldOfView(float fov);
    float getFieldOfView();

private:
    float fieldOfView;
};

