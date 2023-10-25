#pragma once
#include <entityx/System.h>
#include "Mango/Components/TransformComponent.h"

namespace mango
{
    class SceneGraphSystem : public entityx::System<SceneGraphSystem>
    {
    public:
        void configure(entityx::EntityManager& entities, entityx::EventManager& events)                        override;
        void update   (entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt) override;

        static TransformComponent ROOT_NODE;
    };
}
