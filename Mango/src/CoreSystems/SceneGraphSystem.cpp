#include "mgpch.h"
#include "SceneGraphSystem.h"

namespace mango
{
    TransformComponent SceneGraphSystem::ROOT_NODE;

    void SceneGraphSystem::configure(entityx::EntityManager& entities, entityx::EventManager& events)
    {
    }

    void SceneGraphSystem::update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt)
    {
        ROOT_NODE.update(ROOT_NODE.getWorldMatrix(), false);
    }
}
