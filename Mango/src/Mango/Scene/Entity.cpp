#include "Entity.h"
#include "Mango/Scene/Components.h"

namespace mango
{
    UUID Entity::getUUID()
    {
        return getComponent<IDComponent>().id;
    }

    void Entity::setPosition(float x, float y, float z)
    {
        getComponent<TransformComponent>().setPosition(x, y, z);
    }

    void Entity::setPosition(const glm::vec3& position)
    {
        getComponent<TransformComponent>().setPosition(position);
    }
    
    void Entity::setRotation(const glm::vec3& rotation)
    {
        getComponent<TransformComponent>().setRotation(rotation);
    }

    void Entity::setRotation(float x, float y, float z)
    {
        getComponent<TransformComponent>().setRotation(x, y, z);
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

    glm::vec3 Entity::position()
    {
        return getComponent<TransformComponent>().getPosition();
    }

    glm::vec3 Entity::scale()
    {
        return getComponent<TransformComponent>().getScale();
    }

    glm::quat Entity::orientation()
    {
        return getComponent<TransformComponent>().getOrientation();
    }

    void Entity::addChild(Entity& child)
    {
        getComponent<TransformComponent>().addChild(child.getComponent<TransformComponent>());
    }

}