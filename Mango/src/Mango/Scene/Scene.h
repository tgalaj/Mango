#pragma once
#include <string>

#include "entt.hpp"

namespace mango
{
    class Entity;

    class Scene
    {
    public:
        Scene(const std::string& name);
        virtual ~Scene();

        Entity createEntity(const std::string& name = "");
        void destroyEntity(Entity entity);

        std::string& getName() { return m_name; };

        template<typename... Components>
        auto getEntitiesWithComponent()
        {
            return m_registry.view<Components...>();
        }

        bool operator==(const Scene& other)
        {
            return other.m_name == m_name;
        }

    private:
        entt::registry m_registry;
        std::string m_name;

    private:
        friend class Entity;
    };
}

