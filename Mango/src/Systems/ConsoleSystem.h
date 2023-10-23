#pragma once
#include <entityx/System.h>
#include "Events.h"

namespace mango
{
    class ConsoleSystem : public entityx::System<ConsoleSystem>, public entityx::Receiver<ConsoleSystem>
    {
    public:
        void configure(entityx::EventManager& events) override;
        void update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt) override;
        void receive(const InputSystemEvent & event);
        void receive(const entityx::EntityCreatedEvent & event);
    };
}
