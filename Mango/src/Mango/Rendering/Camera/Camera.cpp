#pragma once
#include "mgpch.h"

#include "Camera.h"
#include "Mango/Core/Services.h"
#include "Mango/Window/Window.h"

namespace mango
{
    Camera::Camera()
    {
        m_aspectRatio = Services::application()->getWindow()->getAspectRatio();

        recalculateProjection();
    }

    void Camera::setPerspective(float verticalFov, float aspectRatio, float nearClip, float farClip)
    {
        m_projectionType = ProjectionType::Perspective;
        m_perspectiveFov = verticalFov;
        m_aspectRatio = aspectRatio;
        m_perspectiveNear = nearClip;
        m_perspectiveFar = farClip;
        recalculateProjection();
    }

    void Camera::setOrtographic(float size, float aspectRatio, float nearClip, float farClip)
    {
        m_projectionType = ProjectionType::Orthographic;
        m_orthographicSize = size;
        m_aspectRatio = aspectRatio;
        m_orthographicNear = nearClip;
        m_orthographicFar = farClip;
        recalculateProjection();
    }

    void Camera::recalculateProjection()
    {
        if (m_projectionType == ProjectionType::Perspective)
        {
            m_projection = glm::perspective(m_perspectiveFov, m_aspectRatio, m_perspectiveNear, m_perspectiveFar);
        }
        else
        {
            float left = -m_orthographicSize * m_aspectRatio * 0.5f;
            float right = m_orthographicSize * m_aspectRatio * 0.5f;
            float bottom = -m_orthographicSize * 0.5f;
            float top = m_orthographicSize * 0.5f;

            m_projection = glm::ortho(left, right, bottom, top, m_orthographicNear, m_orthographicFar);
        }
    }

}