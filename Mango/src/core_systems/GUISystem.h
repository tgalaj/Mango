#pragma once
#include <entityx/System.h>

#include "game_logic/BaseGame.h"

namespace mango
{
    class GUISystem : public entityx::System<GUISystem>
    {
    public:
        GUISystem() {}

        void configure(entityx::EventManager & events) override;
        void update(entityx::EntityManager & entities, entityx::EventManager & events, entityx::TimeDelta dt) override;

        void registerGame(const std::shared_ptr<BaseGame> & game);

    private:
        std::shared_ptr<BaseGame> m_game;
    };
}
