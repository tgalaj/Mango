#pragma once
#include <entityx/System.h>
#include "GameLogic/BaseGame.h"

namespace mango
{
    class GUISystem : public entityx::System<GUISystem>
    {
    public:
        GUISystem() {}

        void configure(entityx::EventManager & events) override;
        void update   (entityx::EntityManager & entities, entityx::EventManager & events, entityx::TimeDelta dt) override;
    };
}
