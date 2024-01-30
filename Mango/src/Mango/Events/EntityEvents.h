#pragma once

#include "Event.h"

namespace mango
{
    class Entity;

    struct EntityAddedEvent : Event
    {
        EntityAddedEvent(Entity& e) : entity(e) {}

        std::reference_wrapper<Entity> entity;
    };

    struct EntityRemovedEvent : Event
    {
        EntityRemovedEvent(Entity& e) : entity(e) {}

        std::reference_wrapper<Entity> entity;
    };

    template<typename T>
    struct ComponentAddedEvent : Event
    {
        ComponentAddedEvent(const T& comp, Entity& e)
            : component(comp),
              entity   (e) {}

        T component;
        std::reference_wrapper<Entity> entity;
    };

    template<typename T>
    struct ComponentReplacedEvent : Event
    {
        ComponentReplacedEvent(const T& comp, Entity& e)
            : component(comp),
              entity   (e) {}

        T component;
        std::reference_wrapper<Entity> entity;
    };

    template<typename T>
    struct ComponentRemovedEvent : Event
    {
        ComponentRemovedEvent(const T& comp, Entity& e)
            : component(comp),
              entity   (e) {}

        T component;
        std::reference_wrapper<Entity> entity;
    };
}