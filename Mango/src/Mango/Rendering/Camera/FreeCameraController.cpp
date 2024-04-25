#include "mgpch.h"
#include "FreeCameraController.h"

#include "Mango/Core/Services.h"
#include "Mango/Scene/Entity.h"
#include "Mango/Scene/SceneManager.h"

namespace mango
{
    void FreeCameraController::onUpdate(float dt)
    {
        MG_PROFILE_ZONE_SCOPED;

        // Update the camera's view matrix
        Entity cameraEntity = Services::sceneManager()->getActiveScene()->getPrimaryCamera();

        if (!cameraEntity)
        {
            MG_CORE_WARN("FreeCameraController: no active (primary) camera.");
            return;
        }

        auto& tc = cameraEntity.getComponent<TransformComponent>();
        auto& cc = cameraEntity.getComponent<CameraComponent>();

        glm::mat4 R = glm::mat4_cast(tc.getLocalOrientation());
        glm::mat4 T = glm::translate(glm::mat4(1.0f), -tc.getLocalPosition());

        cc.camera.setView(R * T);

        // Free Move
        auto movementAmount = *CVarSystem::get()->getFloatCVar("camera.moveSpeed") * dt;

        if (Input::getKey(KeyCode::LeftShift))
            movementAmount *= 4.0f;

        if (Input::getKey(forwardKey))
            move(tc, glm::conjugate(tc.getLocalOrientation()) * glm::vec3(0, 0, -1), movementAmount);

        if (Input::getKey(backwardKey))
            move(tc, glm::conjugate(tc.getLocalOrientation()) * glm::vec3(0, 0, 1), movementAmount);

        if (Input::getKey(rightKey))
            move(tc, glm::conjugate(tc.getLocalOrientation()) * glm::vec3(1, 0, 0), movementAmount);

        if (Input::getKey(leftKey))
            move(tc, glm::conjugate(tc.getLocalOrientation()) * glm::vec3(-1, 0, 0), movementAmount);

        if (Input::getKey(upKey))
            move(tc, glm::vec3(0, 1, 0), movementAmount);

        if (Input::getKey(downKey))
            move(tc, glm::vec3(0, -1, 0), movementAmount);

        // Free Look
        if (Input::getMouse(unlockMouseKey))
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

        if (m_isMouseMove)
        {
            auto deltaPos = Input::getMousePosition() - m_mousePressedPosition;

            auto yRot = deltaPos.x != 0.0f;
            auto xRot = deltaPos.y != 0.0f;

            float mouseSensitivity = *CVarSystem::get()->getFloatCVar("camera.rotationSpeed");

            // pitch
            if (xRot)
            {
                tc.setLocalRotation(glm::angleAxis(glm::radians(deltaPos.y * mouseSensitivity), glm::vec3(1, 0, 0)) * tc.getLocalOrientation());
            }

            // yaw
            if (yRot)
            {
                tc.setLocalRotation(tc.getLocalOrientation() * glm::angleAxis(glm::radians(deltaPos.x * mouseSensitivity), glm::vec3(0, 1, 0)));
            }

            if(xRot || yRot)
            {
                m_mousePressedPosition = Input::getMousePosition();
            }
        }

        if (Input::isGamepadPresent(GamepadID::PAD_1))
        {
            float leftTrigger  = Input::getGamepadAxis(GamepadID::PAD_1, GamepadAxis::LEFT_TRIGGER);
            float rightTrigger = Input::getGamepadAxis(GamepadID::PAD_1, GamepadAxis::RIGHT_TRIGGER);
            if (leftTrigger > -1.0f || rightTrigger > -1.0f)
            {
                movementAmount *= 4.0f;
            }

            float leftY = Input::getGamepadAxis(GamepadID::PAD_1, GamepadAxis::LEFT_Y);
            if (glm::abs(leftY) >= gamePadDeadZone)
            {
                move(tc, glm::conjugate(tc.getLocalOrientation()) * glm::vec3(0, 0, 1), leftY * movementAmount);
            }

            float leftX = Input::getGamepadAxis(GamepadID::PAD_1, GamepadAxis::LEFT_X);
            if (glm::abs(leftX) >= gamePadDeadZone)
            {
                move(tc, glm::conjugate(tc.getLocalOrientation()) * glm::vec3(1, 0, 0), leftX * movementAmount);
            }

            if (Input::getGamepadButton(GamepadID::PAD_1, GamepadButton::A))
                move(tc, glm::vec3(0, 1, 0), movementAmount);

            if (Input::getGamepadButton(GamepadID::PAD_1, GamepadButton::B))
                move(tc, glm::vec3(0, -1, 0), movementAmount);

            float rightX = Input::getGamepadAxis(GamepadID::PAD_1, GamepadAxis::RIGHT_X);
            if (glm::abs(rightX) >= gamePadDeadZone)
            {
                tc.setLocalRotation(tc.getLocalOrientation() * glm::angleAxis(glm::radians(rightX * gamePadRotationSensitivity), glm::vec3(0, 1, 0)));
            }

            float rightY = Input::getGamepadAxis(GamepadID::PAD_1, GamepadAxis::RIGHT_Y);
            if (glm::abs(rightY) >= gamePadDeadZone)
            {
                tc.setLocalRotation(glm::angleAxis(glm::radians(rightY * gamePadRotationSensitivity), glm::vec3(1, 0, 0)) * tc.getLocalOrientation());
            }
        }
    }

    void FreeCameraController::move(TransformComponent& transform, const glm::vec3& dir, float amount)
    {
        MG_PROFILE_ZONE_SCOPED;
        transform.setLocalPosition(transform.getLocalPosition() + (dir * amount));
    }

}

