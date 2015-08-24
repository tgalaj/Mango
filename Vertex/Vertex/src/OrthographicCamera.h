#pragma once
#include "Camera.h"
class OrthographicCamera final : public Camera
{
public:
    OrthographicCamera();
    ~OrthographicCamera();

private:
    float zoom;
};

