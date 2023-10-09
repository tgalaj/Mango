#pragma once
#include <entityx/System.h>

#include "core_components/TransformComponent.h"

namespace mango
{
    class FreePoseSystem : public entityx::System<FreePoseSystem>
    {
    public:
        FreePoseSystem()
            : m_mouse_pressed_position(0.0, 0.0),
              m_is_mouse_move(false)
        {}

        void configure(entityx::EntityManager& entities, entityx::EventManager& events) override;
        void update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt) override;

    private:
        void move(TransformComponent & transform, const glm::vec3 & dir, float amount);

        glm::vec2 m_mouse_pressed_position;
        bool m_is_mouse_move;
    };
}
