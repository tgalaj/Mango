#include "core_systems/ConsoleSystem.h"
#include <entityx/entityx.h>

namespace Vertex
{
    void ConsoleSystem::configure(entityx::EventManager& events)
    {
        events.subscribe<InputSystemEvent>(*this);
        events.subscribe<entityx::EntityCreatedEvent>(*this);
    }

    void ConsoleSystem::update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt)
    {
        //static float t = 0.0;
        //static bool flag = true;
        //t += dt;

        //if(t >= 5.0f)
        //{
        //    if (flag)
        //    {
        //        events.emit<InputSystemEvent>();
        //    }
        //    else
        //    {
        //        std::cout << "Console System is alive!" << std::endl;
        //    }

        //    flag = !flag;
        //    t = 0.0f;
        //}
    }

    void ConsoleSystem::receive(const InputSystemEvent& event)
    {
        std::cout << "Input System is alive!" << std::endl;
    }
    void ConsoleSystem::receive(const entityx::EntityCreatedEvent & event)
    {
        //std::cout << "Created Entity ID: " << event.entity.id().id() << std::endl;
    }
}
