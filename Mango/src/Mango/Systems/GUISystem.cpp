#include "mgpch.h"

#include "GUISystem.h"
#include "Mango/Core/CoreServices.h"
#include "Mango/GUI/GUI.h"

namespace mango
{
    void GUISystem::configure(entityx::EventManager& events)
    {
    }

    void GUISystem::update(entityx::EntityManager & entities, entityx::EventManager & events, entityx::TimeDelta dt)
    {
        GUI::prepare();

        CoreServices::getApplication()->getGame()->onGUI(dt);

        GUI::render();
    }
}
