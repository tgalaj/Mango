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

    template<typename... Component>
    static void copyComponent(      entt::registry&                         dst, 
                              const entt::registry&                         src, 
                              const std::unordered_map<UUID, entt::entity>& enttMap)
    {
        ([&]()
        {
            auto view = src.view<Component>();
            for (auto srcEntity : view)
            {
                entt::entity dstEntity = enttMap.at(src.get<IDComponent>(srcEntity).id);

                auto& srcComponent = src.get<Component>(srcEntity);
                dst.emplace_or_replace<Component>(dstEntity, srcComponent);
            }
        }(), ...);
    }

    template<typename... Component>
    static void copyComponent(ComponentsGroup<Component...>, 
                                    entt::registry&                         dst, 
                              const entt::registry&                         src, 
                              const std::unordered_map<UUID, entt::entity>& enttMap)
    {
        copyComponent<Component...>(dst, src, enttMap);
    }

    template<typename... Component>
    static void copyComponentIfExists(Entity dst, Entity src)
    {
        ([&]()
        {
            if (src.hasComponent<Component>())
            {
                dst.addOrReplaceComponent<Component>(src.getComponent<Component>());
            }
        }(), ...);
    }

    template<typename... Component>
    static void copyComponentIfExists(ComponentsGroup<Component...>, Entity dst, Entity src)
    {
        copyComponentIfExists<Component...>(dst, src);
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
                  UUID  uuid = srcSceneRegistry.get<IDComponent>(e).id;
            const auto& name = srcSceneRegistry.get<TagComponent>(e).name;

            Entity newEntity     = newScene->createEntityWithUUID(uuid, name);
                   enttMap[uuid] = (entt::entity)newEntity;
        }

        // Copy components (except ID and Tag components)
        copyComponent(ComponentsRegistry{}, dstSceneRegistry, srcSceneRegistry, enttMap);

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

    Entity Scene::duplicateEntity(Entity entity)
    {
        Entity newEntity = createEntity(entity.getName());

        copyComponentIfExists(ComponentsRegistry{}, newEntity, entity);


        return newEntity;
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