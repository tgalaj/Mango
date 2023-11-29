#pragma once
#include "mgpch.h"

#include "Mango/Core/Services.h"
#include "Mango/Events/EntityEvents.h"
#include "Mango/Scene/Entity.h"
#include "Mango/Scene/Scene.h"
#include "Mango/Scene/SceneManager.h"

namespace mango
{

    CameraComponent::CameraComponent(float fov, float aspectRatio, float zNear, float zFar)
        : m_view      (glm::mat4(1.0f)),
          m_projection(glm::perspective(glm::radians(fov), aspectRatio, zNear, zFar)),
          m_isOrtho   (false)
    {
        // If there's no primary camera yet, set this one as primary
        if (!Services::sceneManager()->getActiveScene()->getPrimaryCamera())
        {
            setPrimary();
        }
    }

    CameraComponent::CameraComponent(float left, float right, float bottom, float top, float zNear, float zFar)
        : m_view      (glm::mat4(1.0f)),
          m_projection(glm::ortho(left, right, bottom, top, zNear, zFar)),
          m_isOrtho   (true)
    {
        // If there's no primary camera yet, set this one as primary
        if (!Services::sceneManager()->getActiveScene()->getPrimaryCamera())
        {
            setPrimary();
        }
    }

    void CameraComponent::setPrimary()
    {
        m_isPrimary = true; 
        Services::application()->getEventBus()->emit(ChangedPrimaryCameraEvent());
    }

}