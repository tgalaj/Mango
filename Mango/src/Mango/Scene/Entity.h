#pragma once

#include "Mango/Core/Services.h"
#include "Mango/Events/EntityEvents.h"
#include "Mango/Scene/Scene.h"

#include "entt.hpp"

namespace mango
{
    class Entity final
    {
    public:
        Entity() = default;
        Entity(entt::entity handle, Scene* scene) : m_entityHandle(handle), m_scene(scene) {}
        Entity(const Entity& other) = default;
        ~Entity() = default;

        template <typename T, typename... Args>
        T& addOrReplaceComponent(Args&&... args)
        {
            T& component = m_scene->m_registry.emplace_or_replace<T>(m_entityHandle, std::forward<Args>(args)...);

            return component;
        }

        template <typename T, typename... Args>
        T& addComponent(Args&&... args)
        {
            MG_CORE_ASSERT_MSG(!hasComponent<T>(), "Entity already has this component!");
            T& component = m_scene->m_registry.emplace<T>(m_entityHandle, std::forward<Args>(args)...);

            Services::eventBus()->emit(ComponentAddedEvent<T>(component, *this));

            return component;
        }

        template <typename T>
        bool hasComponent()
        {
            return m_scene->m_registry.all_of<T>(m_entityHandle);
        }

        template <typename T>
        T& getComponent()
        {
            MG_CORE_ASSERT_MSG(hasComponent<T>(), "Entity doesn't have this component!");
            return m_scene->m_registry.get<T>(m_entityHandle);
        } 

        template<typename T>
        void removeComponent()
        {
            Services::eventBus()->emit(ComponentRemovedEvent<T>(getComponent<T>(), *this));

            MG_CORE_ASSERT_MSG(hasComponent<T>(), "Entity doesn't have this component!");
            m_scene->m_registry.remove<T>(m_entityHandle);
        }

        operator bool()         const { return m_entityHandle != entt::null; }
        operator entt::entity() const { return m_entityHandle; }
        operator uint32_t()     const { return uint32_t(m_entityHandle); }

        bool operator==(const Entity& other) const
        {
            return m_entityHandle == other.m_entityHandle && m_scene == other.m_scene;
        }

        bool operator!=(const Entity& other) const
        {
            return !(*this == other);
        }

        // Shortcuts
        void setPosition(float x, float y, float z);
        void setPosition(const glm::vec3& position);
        
        void setRotation   (float x, float y, float z);
        void setRotation   (const glm::vec3& rotation);
        void setOrientation(const glm::vec3& axis, float angle);
        void setOrientation(const glm::quat& quat);
        
        void setScale(float x, float y, float z);
        void setScale(float uniformScale);

        glm::vec3 position();
        glm::vec3 scale();
        glm::quat orientation();
        
        void addChild(Entity& child);

    private:
        entt::entity m_entityHandle = entt::null;
        Scene* m_scene = nullptr;        

    private:
        friend class Scene;
    };
}
