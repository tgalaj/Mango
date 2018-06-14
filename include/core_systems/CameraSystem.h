#pragma once
#include <entityx/System.h>

namespace Vertex
{
    class CameraSystem : public entityx::System<CameraSystem>
    {
    public:
        void configure(entityx::EntityManager& entities, entityx::EventManager& events) override;
        void update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt) override;
    };
}
