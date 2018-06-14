#pragma once
#include <entityx/System.h>

#include "core_components/TransformComponent.h"

namespace Vertex
{
    class FreePoseSystem : public entityx::System<FreePoseSystem>
    {
    public:
        FreePoseSystem()
            : m_free_look_locked(false)
        {}

        void configure(entityx::EntityManager& entities, entityx::EventManager& events) override;
        void update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt) override;

    private:
        void move(TransformComponent & transform, const glm::vec3 & dir, float amount);

        bool m_free_look_locked;
    };
}
