#pragma once
#include "entt.hpp"

#include <string>

namespace mango
{
    class Entity;
    struct ChangedPrimaryCameraEvent;

    class Scene
    {
    public:
        Scene(const std::string& name);
        virtual ~Scene();

        std::string& getName() { return m_name; };
        
        Entity createEntity(const std::string& name = "Entity");
        void destroyEntity(Entity entity);

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
        void setPrimaryCamera(const ChangedPrimaryCameraEvent & event);

    private:
        entt::registry m_registry;
        std::string m_name;

        entt::entity m_primaryCameraEntityHandle = entt::null;

    private:
        friend class Entity;
        friend class SceneHierarchyPanel;
        friend class SceneSerializer;
    };
}

