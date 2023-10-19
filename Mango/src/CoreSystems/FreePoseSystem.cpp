#include "mgpch.h"

#include "FreePoseSystem.h"
#include "CoreComponents/FreeLookComponent.h"
#include "CoreComponents/FreeMoveComponent.h"
#include "Window/Window.h"

namespace mango
{
    void FreePoseSystem::configure(entityx::EntityManager & entities, entityx::EventManager & events)
    {
    }

    void FreePoseSystem::update(entityx::EntityManager & entities, entityx::EventManager & events, entityx::TimeDelta dt)
    {
        entities.each<FreeMoveComponent, TransformComponent>(
        [this, dt](entityx::Entity entity, FreeMoveComponent & freeMove, TransformComponent & transform)
        {
            auto movementAmount = freeMove.moveSpeed * dt;

            if (Input::getKey(freeMove.forwardKey))
                move(transform, glm::conjugate(transform.getOrientation()) * glm::vec3(0, 0, -1), movementAmount);

            if (Input::getKey(freeMove.backwardKey))
                move(transform, glm::conjugate(transform.getOrientation()) * glm::vec3(0, 0, 1), movementAmount);

            if (Input::getKey(freeMove.rightKey))
                move(transform, glm::conjugate(transform.getOrientation()) * glm::vec3(1, 0, 0), movementAmount);

            if (Input::getKey(freeMove.leftKey))
                move(transform, glm::conjugate(transform.getOrientation()) * glm::vec3(-1, 0, 0), movementAmount);

            if (Input::getKey(freeMove.upKey))
                move(transform, glm::vec3(0, 1, 0), movementAmount);

            if (Input::getKey(freeMove.downKey))
                move(transform, glm::vec3(0, -1, 0), movementAmount);
        });

        entities.each<FreeLookComponent, TransformComponent>(
        [this](entityx::Entity entity, FreeLookComponent & freeLook, TransformComponent & transform)
        {
            if(Input::getMouse(freeLook.unlockMouseKey))
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

                /* pitch */
                if(xRot)
                {
                    transform.setOrientation(glm::angleAxis(glm::radians(deltaPos.y * freeLook.sensitivity), glm::vec3(1, 0, 0)) * transform.getOrientation());
                }

                /* yaw */
                if (yRot)
                {
                    transform.setOrientation(transform.getOrientation() * glm::angleAxis(glm::radians(deltaPos.x * freeLook.sensitivity), glm::vec3(0, 1, 0)));
                }

                if(xRot || yRot)
                {
                    m_mousePressedPosition = Input::getMousePosition();
                }
            }
        });
    }

    void FreePoseSystem::move(TransformComponent& transform, const glm::vec3& dir, float amount)
    {
        transform.setPosition(transform.getPosition() + (dir * amount));
    }
}
