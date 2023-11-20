#pragma once

#include "Event.h"

namespace mango
{
    class Entity;

    template<typename T>
    struct ComponentAddedEvent : Event
    {
        ComponentAddedEvent(T comp, Entity e)
            : component(comp),
              entity(e) {}

        T component;
        Entity entity;
    };

    template<typename T>
    struct ComponentRemovedEvent : Event
    {
        ComponentRemovedEvent(T comp, Entity e)
            : component(comp),
              entity(e) {}

        T component;
        Entity entity;
    };
}