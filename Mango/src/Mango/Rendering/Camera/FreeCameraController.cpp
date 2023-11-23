#include "mgpch.h"
#include "FreeCameraController.h"

#include "Mango/Core/Assertions.h"
#include "Mango/Scene/Components.h"

namespace mango
{
    FreeCameraController::FreeCameraController(Entity& entity)
    {
        MG_CORE_ASSERT_MSG(entity.hasComponent<CameraComponent>(), "entity doesn't have CameraComponent added!");
        m_cameraEntity = entity;   
    }

    void FreeCameraController::onUpdate(float dt)
    {
        // Update the camera's view matrix
        auto& transform = m_cameraEntity.getComponent<TransformComponent>();
        auto& camera    = m_cameraEntity.getComponent<CameraComponent>();

        glm::mat4 R = glm::mat4_cast(transform.getOrientation());
        glm::mat4 T = glm::translate(glm::mat4(1.0f), -transform.getPosition());

        camera.view = R * T;

        // Free Move
        auto movementAmount = moveSpeed * dt;

        if (Input::getKey(KeyCode::LeftShift))
            movementAmount *= 4.0f;

        if (Input::getKey(forwardKey))
            move(transform, glm::conjugate(transform.getOrientation()) * glm::vec3(0, 0, -1), movementAmount);

        if (Input::getKey(backwardKey))
            move(transform, glm::conjugate(transform.getOrientation()) * glm::vec3(0, 0, 1), movementAmount);

        if (Input::getKey(rightKey))
            move(transform, glm::conjugate(transform.getOrientation()) * glm::vec3(1, 0, 0), movementAmount);

        if (Input::getKey(leftKey))
            move(transform, glm::conjugate(transform.getOrientation()) * glm::vec3(-1, 0, 0), movementAmount);

        if (Input::getKey(upKey))
            move(transform, glm::vec3(0, 1, 0), movementAmount);

        if (Input::getKey(downKey))
            move(transform, glm::vec3(0, -1, 0), movementAmount);

        // Free Look
        if(Input::getMouse(unlockMouseKey))
        {
            if (!m_isMouseMove)
            {
                m_mousePressedPosition = Input::getMousePosition();
                Input::setMouseCursorVisibility(false);
            }

            m_isMouseMove = true;
        }
        else
        {
            m_isMouseMove = false;
            Input::setMouseCursorVisibility(true);
        }

        if(m_isMouseMove)
        {
            auto deltaPos = Input::getMousePosition() - m_mousePressedPosition;

            auto yRot = deltaPos.x != 0.0f;
            auto xRot = deltaPos.y != 0.0f;

            // pitch
            if(xRot)
            {
                transform.setOrientation(glm::angleAxis(glm::radians(deltaPos.y * mouseSensitivity), glm::vec3(1, 0, 0)) * transform.getOrientation());
            }

            // yaw
            if (yRot)
            {
                transform.setOrientation(transform.getOrientation() * glm::angleAxis(glm::radians(deltaPos.x * mouseSensitivity), glm::vec3(0, 1, 0)));
            }

            if(xRot || yRot)
            {
                m_mousePressedPosition = Input::getMousePosition();
            }
        }
    }

    CameraComponent& FreeCameraController::getCameraComponent()
    {
        return m_cameraEntity.getComponent<CameraComponent>();
    }

    void FreeCameraController::move(TransformComponent& transform, const glm::vec3& dir, float amount)
    {
        transform.setPosition(transform.getPosition() + (dir * amount));
    }

}

