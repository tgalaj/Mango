#include "Entity.h"
#include "Mango/Scene/Components.h"

namespace mango
{
    const Entity Entity::nullEntity = { entt::null, nullptr };

    UUID Entity::getUUID()
    {
        return getComponent<IDComponent>().id;
    }

    const std::string& Entity::getName()
    {
        return getComponent<TagComponent>().name;
    }

    void Entity::setLocalPosition(float x, float y, float z)
    {
        getComponent<TransformComponent>().setLocalPosition(x, y, z);
    }

    void Entity::setLocalPosition(const glm::vec3& position)
    {
        getComponent<TransformComponent>().setLocalPosition(position);
    }
    
    void Entity::setLocalRotation(const glm::vec3& rotation)
    {
        getComponent<TransformComponent>().setLocalRotation(rotation);
    }

    void Entity::setLocalRotation(float x, float y, float z)
    {
        getComponent<TransformComponent>().setLocalRotation(x, y, z);
    }

    void Entity::setLocalOrientation(const glm::vec3& axis, float angle)
    {
        getComponent<TransformComponent>().setLocalRotation(axis, angle);
    }

    void Entity::setLocalOrientation(const glm::quat& quat)
    {
        getComponent<TransformComponent>().setLocalRotation(quat);
    }

    void Entity::setLocalScale(float x, float y, float z)
    {
        getComponent<TransformComponent>().setLocalScale(x, y, z);
    }

    void Entity::setLocalScale(float uniformScale)
    {
        getComponent<TransformComponent>().setLocalScale(uniformScale);
    }

    glm::vec3 Entity::getLocalPosition()
    {
        return getComponent<TransformComponent>().getLocalPosition();
    }

    glm::vec3 Entity::getLocalScale()
    {
        return getComponent<TransformComponent>().getLocalScale();
    }

    glm::vec3 Entity::getLocalRotation()
    {
        return getComponent<TransformComponent>().getLocalRotation();
    }

    glm::quat Entity::getLocalOrientation()
    {
        return getComponent<TransformComponent>().getLocalOrientation();
    }

    void Entity::setPosition(float x, float y, float z)
    {
        getComponent<TransformComponent>().setPosition(x, y, z);
    }

    void Entity::setPosition(const glm::vec3& position)
    {
        getComponent<TransformComponent>().setPosition(position);
    }

    void Entity::setRotation(float x, float y, float z)
    {
        getComponent<TransformComponent>().setRotation(x, y, z);
    }

    void Entity::setRotation(const glm::vec3& rotation)
    {
        getComponent<TransformComponent>().setRotation(rotation);
    }

    void Entity::setOrientation(const glm::vec3& axis, float angle)
    {
        getComponent<TransformComponent>().setRotation(axis, angle);
    }

    void Entity::setOrientation(const glm::quat& quat)
    {
        getComponent<TransformComponent>().setRotation(quat);
    }

    glm::vec3 Entity::getPosition()
    {
        return getComponent<TransformComponent>().getPosition();
    }

    glm::vec3 Entity::getScale()
    {
        return getComponent<TransformComponent>().getScale();
    }

    glm::vec3 Entity::getRotation()
    {
        return getComponent<TransformComponent>().getRotation();
    }

    glm::quat Entity::getOrientation()
    {
        return getComponent<TransformComponent>().getOrientation();
    }

    glm::vec3 Entity::getForward()
    {
        return getComponent<TransformComponent>().getForward();
    }

    glm::vec3 Entity::getBackward()
    {
        return getComponent<TransformComponent>().getBackward();
    }

    glm::vec3 Entity::getRight()
    {
        return getComponent<TransformComponent>().getRight();
    }

    glm::vec3 Entity::getUp()
    {
        return getComponent<TransformComponent>().getUp();
    }

    TransformComponent& Entity::getTransform()
    {
        return getComponent<TransformComponent>();
    }

    TransformComponent& Entity::getParentTransform()
    {
        MG_CORE_ASSERT(hasParent());

        return getParent().getComponent<TransformComponent>();
    }

    Entity Entity::getParent()
    {
        return getComponent<TransformComponent>().getParent();
    }

    bool Entity::hasParent()
    {
        if (!hasComponent<TransformComponent>()) return false;

        return getComponent<TransformComponent>().hasParent();
    }

    void Entity::addChild(Entity child)
    {
        getComponent<TransformComponent>().addChild(child, *this);
    }

    void Entity::removeChild(Entity child)
    {
        getComponent<TransformComponent>().removeChild(child);
    }

    Entity Entity::findChild(Entity child)
    {
        auto children = getChildren();

        auto it = std::find(children.begin(), children.end(), child);
        if (it != children.end())
        {
            return *it;
        }

        for (auto c : children)
        {
            return c.findChild(child);
        }

        return nullEntity;
    }

    bool Entity::hasChild(Entity child)
    {
        return findChild(child) != nullEntity;
    }

    std::vector<Entity>& Entity::getChildren()
    {
        return getComponent<TransformComponent>().getChildren();
    }

}