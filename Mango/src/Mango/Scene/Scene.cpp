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

    Scene::~Scene()
    {

    }

    Entity Scene::createEntity(const std::string& name)
    {
        Entity entity = { m_registry.create(), this };
        entity.addComponent<TransformComponent>();

        SceneGraphSystem::ROOT_NODE.addChild(entity.getComponent<TransformComponent>());

        return entity;
    }

    void Scene::destroyEntity(Entity entity)
    {
        m_registry.destroy(entity);
    }

}