#include "EditorCamera.h"
#include "Mango.h"

namespace mango
{

    EditorCamera::EditorCamera()
    {
        updateView();
    }

    void EditorCamera::onUpdate(float dt)
    {
        MG_PROFILE_ZONE_SCOPED;

        // Unlock the camera movement when user pressed the unlockMouseKey
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
            updateView();

            // Free Move
            auto movementAmount = *CVarSystem::get()->getFloatCVar("editor.camera.moveSpeed") * dt;

            if (Input::getKey(KeyCode::LeftShift))
                movementAmount *= 4.0f;

            if (Input::getKey(forwardKey))
                move(glm::conjugate(m_orientation) * glm::vec3(0, 0, -1), movementAmount);

            if (Input::getKey(backwardKey))
                move(glm::conjugate(m_orientation) * glm::vec3(0, 0, 1), movementAmount);

            if (Input::getKey(rightKey))
                move(glm::conjugate(m_orientation) * glm::vec3(1, 0, 0), movementAmount);

            if (Input::getKey(leftKey))
                move(glm::conjugate(m_orientation) * glm::vec3(-1, 0, 0), movementAmount);

            if (Input::getKey(upKey))
                move(glm::vec3(0, 1, 0), movementAmount);

            if (Input::getKey(downKey))
                move(glm::vec3(0, -1, 0), movementAmount);

            // Free look
            auto deltaPos = Input::getMousePosition() - m_mousePressedPosition;

            auto yRot = deltaPos.x != 0.0f;
            auto xRot = deltaPos.y != 0.0f;

            float mouseSensitivity = *CVarSystem::get()->getFloatCVar("editor.camera.rotationSpeed");

            // pitch
            if (xRot)
            {
                m_orientation = glm::normalize(glm::angleAxis(glm::radians(deltaPos.y * mouseSensitivity), glm::vec3(1, 0, 0)) * m_orientation);
            }

            // yaw
            if (yRot)
            {
                m_orientation = glm::normalize(m_orientation * glm::angleAxis(glm::radians(deltaPos.x * mouseSensitivity), glm::vec3(0, 1, 0)));
            }

            if (xRot || yRot)
            {
                m_mousePressedPosition = Input::getMousePosition();
            }
            
            // Gamepad look and move
            if (Input::isGamepadPresent(GamepadID::PAD_1))
            {
                float leftTrigger = Input::getGamepadAxis(GamepadID::PAD_1, GamepadAxis::LEFT_TRIGGER);
                float rightTrigger = Input::getGamepadAxis(GamepadID::PAD_1, GamepadAxis::RIGHT_TRIGGER);
                if (leftTrigger > -1.0f || rightTrigger > -1.0f)
                {
                    movementAmount *= 4.0f;
                }

                float leftY = Input::getGamepadAxis(GamepadID::PAD_1, GamepadAxis::LEFT_Y);
                if (glm::abs(leftY) >= gamePadDeadZone)
                {
                    move(glm::conjugate(m_orientation) * glm::vec3(0, 0, 1), leftY * movementAmount);
                }

                float leftX = Input::getGamepadAxis(GamepadID::PAD_1, GamepadAxis::LEFT_X);
                if (glm::abs(leftX) >= gamePadDeadZone)
                {
                    move(glm::conjugate(m_orientation) * glm::vec3(1, 0, 0), leftX * movementAmount);
                }

                if (Input::getGamepadButton(GamepadID::PAD_1, GamepadButton::A))
                    move(glm::vec3(0, 1, 0), movementAmount);

                if (Input::getGamepadButton(GamepadID::PAD_1, GamepadButton::B))
                    move(glm::vec3(0, -1, 0), movementAmount);

                float rightX = Input::getGamepadAxis(GamepadID::PAD_1, GamepadAxis::RIGHT_X);
                if (glm::abs(rightX) >= gamePadDeadZone)
                {
                    m_orientation = glm::normalize(m_orientation * glm::angleAxis(glm::radians(rightX * gamePadRotationSensitivity), glm::vec3(0, 1, 0)));
                }

                float rightY = Input::getGamepadAxis(GamepadID::PAD_1, GamepadAxis::RIGHT_Y);
                if (glm::abs(rightY) >= gamePadDeadZone)
                {
                    m_orientation = glm::normalize(glm::angleAxis(glm::radians(rightY * gamePadRotationSensitivity), glm::vec3(1, 0, 0)) * m_orientation);
                }
            }
        }
    }

    void EditorCamera::move(const glm::vec3& dir, float amount)
    {
        MG_PROFILE_ZONE_SCOPED;
        m_position += dir * amount;
    }

    void EditorCamera::updateView()
    {
        glm::mat4 R = glm::mat4_cast(m_orientation);
        glm::mat4 T = glm::translate(glm::mat4(1.0f), -m_position);

        m_view = R * T;
    }

}