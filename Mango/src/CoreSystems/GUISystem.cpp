#include "mgpch.h"

#include "CoreSystems/GUISystem.h"
#include "GUI/GUI.h"

namespace mango
{
    void GUISystem::configure(entityx::EventManager& events)
    {
    }

    void GUISystem::update(entityx::EntityManager & entities, entityx::EventManager & events, entityx::TimeDelta dt)
    {
        GUI::prepare();

        m_game->onGUI(dt);

        GUI::render();
    }

    void GUISystem::registerGame(const std::shared_ptr<BaseGame> & game)
    {
        m_game = game;
    }
}
