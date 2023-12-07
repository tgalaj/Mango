#pragma once

#include "Mango/Scene/Entity.h"
#include "Mango/Window/Input.h"

namespace mango
{
    class CameraComponent;
    class TransformComponent;

    class FreeCameraController
    {
        public:
            explicit FreeCameraController(Entity& entity);
            virtual ~FreeCameraController() = default;

            void onUpdate(float dt);

            CameraComponent& getCameraComponent();
            void setCameraEntity(Entity entity);

            float   mouseSensitivity = 0.2f;
            KeyCode unlockMouseKey   = KeyCode::MouseRight;

            float   moveSpeed   = 10.0f;
            KeyCode forwardKey  = KeyCode::W;
            KeyCode backwardKey = KeyCode::S;
            KeyCode leftKey     = KeyCode::A;
            KeyCode rightKey    = KeyCode::D;
            KeyCode upKey       = KeyCode::E;
            KeyCode downKey     = KeyCode::Q;

            float gamePadDeadZone = 0.3f;
            float gamePadRotationSensitivity = 0.1f;

        private:
            void move(TransformComponent& transform, const glm::vec3& dir, float amount);

        private:
            Entity    m_cameraEntity;
            glm::vec2 m_mousePressedPosition{ 0.0f, 0.0f };
            bool      m_isMouseMove{ false };
    };
}