
#include <core_engine/GameObject.h>

#include "core_components/TransformComponent.h"
#include "core_systems/SceneGraphSystem.h"
#include "core_engine/CoreServices.h"

namespace Vertex
{
    GameObject::GameObject()
    {
        entity = CoreServices::getCore()->entities.create();
        addComponent<TransformComponent>();

        SceneGraphSystem::M_ROOT_NODE.addChild(entity.component<TransformComponent>());
    }

    void GameObject::setPosition(float x, float y, float z)
    {
        entity.component<TransformComponent>()->setPosition(x, y, z);
    }

    void GameObject::setPosition(const glm::vec3 & position)
    {
        entity.component<TransformComponent>()->setPosition(position);
    }

    void GameObject::setOrientation(float x, float y, float z)
    {
        entity.component<TransformComponent>()->setOrientation(x, y, z);
    }

    void GameObject::setOrientation(const glm::vec3 & axis, float angle)
    {
        entity.component<TransformComponent>()->setOrientation(axis, angle);
    }

    void GameObject::setOrientation(const glm::quat & quat)
    {
        entity.component<TransformComponent>()->setOrientation(quat);
    }

    void GameObject::setScale(float x, float y, float z)
    {
        entity.component<TransformComponent>()->setScale(x, y, z);
    }

    void GameObject::setScale(float uniform_scale)
    {
        entity.component<TransformComponent>()->setScale(uniform_scale);
    }

    void GameObject::addChild(GameObject & child)
    {
        entity.component<TransformComponent>()->addChild(child.getComponent<TransformComponent>());
    }
}
