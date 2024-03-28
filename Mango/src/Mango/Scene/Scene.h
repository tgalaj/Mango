#pragma once
#include "Mango/Core/UUID.h"

#include <entt.hpp>

#include <string>

namespace mango
{
    class Entity;

    class Scene
    {
    public:
        Scene(const std::string& name = "New Scene");
        virtual ~Scene() = default;

        static ref<Scene> copy(ref<Scene>& other);

        std::string& getName() { return m_name; };
        
        Entity createEntity(const std::string& name = "Entity");
        Entity createEntityWithUUID(UUID uuid, const std::string& name = "Entity");

        Entity duplicateEntity(Entity entity);
        void destroyEntity(Entity entity);

        Entity getEntity(UUID uuid);
        Entity getPrimaryCamera();

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
        friend class SceneHierarchyPanel;
        friend class SceneSerializer;
    };
}

