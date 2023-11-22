#include "Entity.h"
#include "Mango/Scene/Components.h"

namespace mango
{

    void Entity::setPosition(float x, float y, float z)
    {
        getComponent<TransformComponent>().setPosition(x, y, z);
    }

    void Entity::setPosition(const glm::vec3& position)
    {
        getComponent<TransformComponent>().setPosition(position);
    }

    void Entity::setOrientation(float x, float y, float z)
    {
        getComponent<TransformComponent>().setOrientation(x, y, z);
    }

    void Entity::setOrientation(const glm::vec3& axis, float angle)
    {
        getComponent<TransformComponent>().setOrientation(axis, angle);
    }

    void Entity::setOrientation(const glm::quat& quat)
    {
        getComponent<TransformComponent>().setOrientation(quat);
    }

    void Entity::setScale(float x, float y, float z)
    {
        getComponent<TransformComponent>().setScale(x, y, z);
    }

    void Entity::setScale(float uniformScale)
    {
        getComponent<TransformComponent>().setScale(uniformScale);
    }

    void Entity::addChild(Entity& child)
    {
        getComponent<TransformComponent>().addChild(child.getComponent<TransformComponent>());
    }

}