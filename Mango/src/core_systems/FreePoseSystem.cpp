#include "mgpch.h"

#include "core_systems/FreePoseSystem.h"
#include "core_components/FreeMoveComponent.h"
#include "core_components/FreeLookComponent.h"
#include "window/Window.h"

namespace mango
{
    void FreePoseSystem::configure(entityx::EntityManager & entities, entityx::EventManager & events)
    {
    }

    void FreePoseSystem::update(entityx::EntityManager & entities, entityx::EventManager & events, entityx::TimeDelta dt)
    {
        entities.each<FreeMoveComponent, TransformComponent>(
        [this, dt](entityx::Entity entity, FreeMoveComponent & free_move, TransformComponent & transform)
        {
            auto movement_amount = free_move.m_move_speed * dt;

            if (Input::getKey(free_move.m_forward_key))
                move(transform, glm::conjugate(transform.orientation()) * glm::vec3(0, 0, -1), movement_amount);

            if (Input::getKey(free_move.m_backward_key))
                move(transform, glm::conjugate(transform.orientation()) * glm::vec3(0, 0, 1), movement_amount);

            if (Input::getKey(free_move.m_right_key))
                move(transform, glm::conjugate(transform.orientation()) * glm::vec3(1, 0, 0), movement_amount);

            if (Input::getKey(free_move.m_left_key))
                move(transform, glm::conjugate(transform.orientation()) * glm::vec3(-1, 0, 0), movement_amount);

            if (Input::getKey(free_move.m_up_key))
                move(transform, glm::vec3(0, 1, 0), movement_amount);

            if (Input::getKey(free_move.m_down_key))
                move(transform, glm::vec3(0, -1, 0), movement_amount);
        });

        entities.each<FreeLookComponent, TransformComponent>(
        [this](entityx::Entity entity, FreeLookComponent & free_look, TransformComponent & transform)
        {
            if(Input::getMouse(free_look.m_unlock_mouse_key))
            {
                if (!m_is_mouse_move)
                {
                    m_mouse_pressed_position = Input::getMousePosition();
                    Input::setMouseCursorVisibility(false);
                }

                m_is_mouse_move = true;
            }
            else
            {
                m_is_mouse_move = false;
                Input::setMouseCursorVisibility(true);
            }

            if(m_is_mouse_move)
            {
                auto delta_pos = Input::getMousePosition() - m_mouse_pressed_position;

                auto rot_y = delta_pos.x != 0.0f;
                auto rot_x = delta_pos.y != 0.0f;

                /* pitch */
                if(rot_x)
                {
                    transform.setOrientation(glm::angleAxis(glm::radians(delta_pos.y * free_look.m_sensitivity), glm::vec3(1, 0, 0)) * transform.orientation());
                }

                /* yaw */
                if (rot_y)
                {
                    transform.setOrientation(transform.orientation() * glm::angleAxis(glm::radians(delta_pos.x * free_look.m_sensitivity), glm::vec3(0, 1, 0)));
                }

                if(rot_x || rot_y)
                {
                    m_mouse_pressed_position = Input::getMousePosition();
                }
            }
        });
    }

    void FreePoseSystem::move(TransformComponent& transform, const glm::vec3& dir, float amount)
    {
        transform.setPosition(transform.position() + (dir * amount));
    }
}
