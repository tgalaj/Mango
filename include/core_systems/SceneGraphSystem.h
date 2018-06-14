#pragma once
#include <entityx/System.h>

#include "core_components/TransformComponent.h"

namespace Vertex
{
    class SceneGraphSystem : public entityx::System<SceneGraphSystem>
    {
    public:
        void configure(entityx::EntityManager& entities, entityx::EventManager& events) override;
        void update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt) override;

        static TransformComponent M_ROOT_NODE;
    };
}
