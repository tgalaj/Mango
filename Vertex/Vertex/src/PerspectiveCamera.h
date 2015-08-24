#pragma once
#include "Camera.h"
class PerspectiveCamera final : public Camera
{
public:
    PerspectiveCamera();
    ~PerspectiveCamera();

private:
    float fieldOfView;
};

