#include "Components.h"

namespace mango
{
    void TransformComponent::addChild(Entity childEntity, Entity parentEntity)
    {
        auto& childTransform          = childEntity.getComponent<TransformComponent>();
              childTransform.m_parent = parentEntity;

        m_children.push_back(childEntity);

        // Calculate new position for the attached child
        // so it's world space position is the same
        // TODO ...
    }

    void TransformComponent::removeChild(Entity childEntity)
    {
        auto it = std::find(m_children.begin(), m_children.end(), childEntity);

        if (it != m_children.end())
        {
            childEntity.getComponent<TransformComponent>().resetParent();
            m_children.erase(it);
        }
    }

    void TransformComponent::resetParent()
    {
        m_parent            = Entity::nullEntity;
        m_parentWorldMatrix = glm::mat4(1.0f);

        glm::vec3 rotation;
        math::decompose(m_worldMatrix, m_position, rotation, m_scale);
        setRotation(rotation);
    }

    void TransformComponent::update(const glm::mat4& parentTransform, bool dirty)
    {
        dirty |= m_dirty;

        if (dirty)
        {
            m_parentWorldMatrix = parentTransform;
            m_localWorldMatrix  = getUpdatedWorldMatrix();
            m_worldMatrix       = m_parentWorldMatrix * m_localWorldMatrix;
            m_normalMatrix      = glm::mat3(glm::transpose(glm::inverse(m_worldMatrix)));

            m_dirty = false;
        }

        for (unsigned i = 0; i < m_children.size(); ++i)
        {
            m_children[i].getComponent<TransformComponent>().update(m_worldMatrix, dirty);
        }
    }
}