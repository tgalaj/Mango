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
              tag.tag   = name;
        auto& transform = entity.addComponent<TransformComponent>();

        return entity;
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