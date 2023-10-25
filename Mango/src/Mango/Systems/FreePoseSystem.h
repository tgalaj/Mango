#pragma once
#include <entityx/System.h>

#include "Mango/Components/TransformComponent.h"

namespace mango
{
    class FreePoseSystem : public entityx::System<FreePoseSystem>
    {
    public:
        FreePoseSystem()
            : m_mousePressedPosition(0.0, 0.0),
              m_isMouseMove         (false)
        {}

        void configure(entityx::EntityManager & entities, entityx::EventManager & events)                        override;
        void update   (entityx::EntityManager & entities, entityx::EventManager & events, entityx::TimeDelta dt) override;

    private:
        void move(TransformComponent & transform, const glm::vec3 & dir, float amount);

        glm::vec2 m_mousePressedPosition{};
        bool m_isMouseMove;
    };
}
