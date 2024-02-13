#pragma once
#include "Mango/Rendering/Camera/Camera.h"
#include "Mango/Window/Input.h"

#include <glm/gtc/quaternion.hpp>

namespace mango
{
    class EditorCamera : public Camera
    {
    public:
        EditorCamera();
        virtual ~EditorCamera() = default;

        void onUpdate(float dt);

        KeyCode unlockMouseKey = KeyCode::MouseRight;

        KeyCode forwardKey  = KeyCode::W;
        KeyCode backwardKey = KeyCode::S;
        KeyCode leftKey     = KeyCode::A;
        KeyCode rightKey    = KeyCode::D;
        KeyCode upKey       = KeyCode::E;
        KeyCode downKey     = KeyCode::Q;

        float gamePadDeadZone = 0.3f;
        float gamePadRotationSensitivity = 0.1f;

        glm::vec3 getPosition() const { return m_position; }
        void setPosition(const glm::vec3& position) { m_position = position; updateView(); }

    private:
        void move(const glm::vec3& dir, float amount);
        void updateView();

    private:
        glm::quat m_orientation          { 1.0f, 0.0f, 0.0f, 0.0f }; // identity quaternion
        glm::vec3 m_position             { 0.0f, 0.0f, 0.0f };
        glm::vec2 m_mousePressedPosition { 0.0f, 0.0f };
        bool      m_isMouseMove          { false };
    };
}