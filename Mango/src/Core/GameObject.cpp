#include "mgpch.h"

#include "GameObject.h"
#include "Components/TransformComponent.h"
#include "CoreServices.h"
#include "Systems/SceneGraphSystem.h"

namespace mango
{
    GameObject::GameObject()
    {
        entity = CoreServices::getApplication()->entities.create();
        addComponent<TransformComponent>();

        SceneGraphSystem::ROOT_NODE.addChild(entity.component<TransformComponent>());
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

    void GameObject::setScale(float uniformScale)
    {
        entity.component<TransformComponent>()->setScale(uniformScale);
    }

    void GameObject::addChild(GameObject & child)
    {
        entity.component<TransformComponent>()->addChild(child.getComponent<TransformComponent>());
    }
}
