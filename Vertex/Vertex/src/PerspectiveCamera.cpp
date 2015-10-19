#include "PerspectiveCamera.h"
#include <glm\gtc\matrix_transform.hpp>

PerspectiveCamera::PerspectiveCamera(float _fieldOfView, float _viewportWidth, float _viewportHeight, float _nearPlane, float _farPlane)
{
    fieldOfView    = _fieldOfView;
    viewportWidth  = _viewportWidth;
    viewportHeight = _viewportHeight;
    nearPlane      = _nearPlane;
    farPlane       = _farPlane;

    update();
}


PerspectiveCamera::~PerspectiveCamera()
{
}

void PerspectiveCamera::update()
{
    direction  = lookAtPoint - position;

    view       = glm::lookAt(position, lookAtPoint, up);
    projection = glm::perspective(glm::radians(fieldOfView), viewportWidth/viewportHeight, nearPlane, farPlane);

    viewProjection = projection * view;

    isDirty = false;
}
