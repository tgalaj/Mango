#include "mgpch.h"

#include "CoreSystems/SceneGraphSystem.h"

namespace mango
{
    TransformComponent SceneGraphSystem::M_ROOT_NODE;

    void SceneGraphSystem::configure(entityx::EntityManager& entities, entityx::EventManager& events)
    {
    }

    void SceneGraphSystem::update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt)
    {
        M_ROOT_NODE.update(M_ROOT_NODE.world_matrix(), false);
    }
}
