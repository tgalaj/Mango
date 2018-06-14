#include "core_systems/FreePoseSystem.h"
#include "core_components/FreeMoveComponent.h"
#include "core_components/FreeLookComponent.h"
#include "framework/window/Window.h"

namespace Vertex
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
            if(/*Input::getKey(free_look.m_unlock_mouse_key) ||*/ Input::getMouseDown(free_look.m_unlock_mouse_key))
            {
                m_free_look_locked = !m_free_look_locked;
                Input::setMouseCursorVisibility(!m_free_look_locked);

                if (m_free_look_locked)
                {
                    Input::setMouseCursorPosition(Window::getCenter());
                }
            }

            if(m_free_look_locked)
            {
                auto delta_pos = Input::getMousePosition() - Window::getCenter();

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
                    Input::setMouseCursorPosition(Window::getCenter());
                }
            }
        });
    }

    void FreePoseSystem::move(TransformComponent& transform, const glm::vec3& dir, float amount)
    {
        transform.setPosition(transform.position() + (dir * amount));
    }
}
