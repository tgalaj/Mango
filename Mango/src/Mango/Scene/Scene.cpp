#include "mgpch.h"
#include "Entity.h"
#include "Scene.h"
#include "Mango/Systems/SceneGraphSystem.h"

namespace mango
{
    Scene::Scene(const std::string& name)
        : m_name(name)
    {
        Services::eventBus()->subscribe<ChangedPrimaryCameraEvent>(MG_BIND_EVENT(setPrimaryCamera));
    }

    Scene::~Scene()
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

        SceneGraphSystem::ROOT_NODE.addChild(transform);

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
        return { m_primaryCameraEntityHandle, this };
    }

    void Scene::setPrimaryCamera(const ChangedPrimaryCameraEvent& event)
    {
        auto view = m_registry.view<CameraComponent>();

        auto prevPrimaryCameraEntityHandle = m_primaryCameraEntityHandle;

        // Find the new entity that has primary flag set
        // and which the entity handle is different then previously 
        // cached handle
        for (auto entity : view)
        {
            auto& camera = view.get<CameraComponent>(entity);
            if (camera.isPrimary() && entity != prevPrimaryCameraEntityHandle)
            {
                m_primaryCameraEntityHandle = entity;
                break;
            }
        }

        if (prevPrimaryCameraEntityHandle != entt::null && prevPrimaryCameraEntityHandle != m_primaryCameraEntityHandle)
        {
            auto& cameraComponent = view.get<CameraComponent>(prevPrimaryCameraEntityHandle);
            cameraComponent.m_isPrimary = false;
        }
    }

}