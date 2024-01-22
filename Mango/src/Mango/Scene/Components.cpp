#pragma once
#include "mgpch.h"

#include "Mango/Core/Services.h"
#include "Mango/Events/EntityEvents.h"
#include "Mango/Scene/Entity.h"
#include "Mango/Scene/Scene.h"
#include "Mango/Scene/SceneManager.h"

namespace mango
{
    CameraComponent::CameraComponent()
    {
        recalculateProjection();

        // If there's no primary camera yet, set this one as primary
        if (!Services::sceneManager()->getActiveScene()->getPrimaryCamera())
        {
            setPrimary();
        }
    }

    void CameraComponent::setPerspective(float verticalFov, float aspectRatio, float nearClip, float farClip)
    {
        m_projectionType  = ProjectionType::Perspective;
        m_perspectiveFov  = glm::radians(verticalFov);
        m_aspectRatio     = aspectRatio;
        m_perspectiveNear = nearClip;
        m_perspectiveFar  = farClip;
        recalculateProjection();
    }

    void CameraComponent::setOrtographic(float size, float aspectRatio, float nearClip, float farClip)
    {
        m_projectionType   = ProjectionType::Orthographic;
        m_orthographicSize = size;
        m_aspectRatio      = aspectRatio;
        m_orthographicNear = nearClip;
        m_orthographicFar  = farClip;
        recalculateProjection();
    }

    void CameraComponent::setPrimary()
    {
        m_isPrimary = true; 
        Services::application()->getEventBus()->emit(ChangedPrimaryCameraEvent()); // todo: make m_freeCameraController react on this event
    }

    void CameraComponent::recalculateProjection()
    {
        if (m_projectionType == ProjectionType::Perspective)
        {
            m_projection = glm::perspective(m_perspectiveFov, m_aspectRatio, m_perspectiveNear, m_perspectiveFar);
        }
        else
        {
            float left   = -m_orthographicSize * m_aspectRatio * 0.5f;
            float right  =  m_orthographicSize * m_aspectRatio * 0.5f;
            float bottom = -m_orthographicSize * 0.5f;
            float top    =  m_orthographicSize * 0.5f;

            m_projection = glm::ortho(left, right, bottom, top, m_orthographicNear, m_orthographicFar);
        }
    }

}