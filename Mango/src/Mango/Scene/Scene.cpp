#include "mgpch.h"
#include "Entity.h"
#include "Scene.h"
#include "Mango/Systems/SceneGraphSystem.h"

namespace mango
{
    Scene::Scene(const std::string& name)
        : m_name(name)
    {
    }

    template<typename Component>
    static void copyComponent(entt::registry& dst, const entt::registry& src, const std::unordered_map<UUID, entt::entity>& enttMap)
    {
        auto view = src.view<Component>();
        for (auto e : view)
        {
            UUID uuid = src.get<IDComponent>(e).id;

            MG_CORE_ASSERT(enttMap.find(uuid) != enttMap.end());

            entt::entity dstEnttID = enttMap.at(uuid);

            auto& component = src.get<Component>(e);
            dst.emplace_or_replace<Component>(dstEnttID, component);
        }
    }

    template<typename Component>
    static void copyComponentIfExists(Entity dst, Entity src)
    {
        if (src.hasComponent<Component>())
        {
            dst.addOrReplaceComponent<Component>(src.getComponent<Component>());
        }
    }

    std::shared_ptr<Scene> Scene::copy(std::shared_ptr<Scene>& other)
    {
        std::shared_ptr<Scene> newScene = std::make_shared<Scene>();

        newScene->m_name = other->m_name;

        auto& srcSceneRegistry = other->m_registry;
        auto& dstSceneRegistry = newScene->m_registry;
        std::unordered_map<UUID, entt::entity> enttMap;

        // Create entitites in new scene
        auto idView = srcSceneRegistry.view<IDComponent>();
        for (auto e : idView)
        {
            UUID uuid = srcSceneRegistry.get<IDComponent>(e).id;
            const auto& name = srcSceneRegistry.get<TagComponent>(e).name;

            Entity newEntity = newScene->createEntityWithUUID(uuid, name);
            enttMap[uuid] = (entt::entity)newEntity;
        }

        // Find entity with ID 
        // Copy components (except ID and Tag components)
        copyComponent<DirectionalLightComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
        copyComponent<PointLightComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
        copyComponent<SpotLightComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
        copyComponent<CameraComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
        copyComponent<ModelRendererComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
        copyComponent<TransformComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
        copyComponent<RigidBody3DComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
        copyComponent<SphereColliderComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);
        copyComponent<BoxCollider3DComponent>(dstSceneRegistry, srcSceneRegistry, enttMap);

        return newScene;
    }

    Entity Scene::createEntity(const std::string& name)
    {
        MG_PROFILE_ZONE_SCOPED;

        return createEntityWithUUID(UUID(), name);
    }

    mango::Entity Scene::createEntityWithUUID(UUID uuid, const std::string& name /*= "Entity"*/)
    {
        MG_PROFILE_ZONE_SCOPED;
        Entity entity = { m_registry.create(), this };
        
                          entity.addComponent<IDComponent>(uuid);
        auto& tag       = entity.addComponent<TagComponent>();
              tag.name  = name;
        auto& transform = entity.addComponent<TransformComponent>();

        return entity;
    }

    void Scene::duplicateEntity(Entity entity)
    {
        Entity newEntity = createEntity(entity.getName());

        copyComponentIfExists<DirectionalLightComponent>(newEntity, entity);
        copyComponentIfExists<PointLightComponent>(newEntity, entity);
        copyComponentIfExists<SpotLightComponent>(newEntity, entity);
        copyComponentIfExists<CameraComponent>(newEntity, entity);
        copyComponentIfExists<ModelRendererComponent>(newEntity, entity);
        copyComponentIfExists<TransformComponent>(newEntity, entity);
        copyComponentIfExists<RigidBody3DComponent>(newEntity, entity);
        copyComponentIfExists<SphereColliderComponent>(newEntity, entity);
        copyComponentIfExists<BoxCollider3DComponent>(newEntity, entity);
    }

    void Scene::destroyEntity(Entity entity)
    {
        MG_PROFILE_ZONE_SCOPED;
        Services::eventBus()->emit(EntityRemovedEvent(entity));
        m_registry.destroy(entity);
    }

    Entity Scene::getPrimaryCamera()
    {
        auto view = m_registry.view<CameraComponent>();
        for (auto entity : view)
        {
            auto& cc = view.get<CameraComponent>(entity);
            if (cc.isPrimary)
            {
                return { entity, this };
            }
        }
        return {};
    }
}