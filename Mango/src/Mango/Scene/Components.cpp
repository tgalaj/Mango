#include "Components.h"

namespace mango
{
    void TransformComponent::addChild(Entity childEntity, Entity parentEntity)
    {
        auto& childTransform          = childEntity.getComponent<TransformComponent>();
              childTransform.m_parent = parentEntity;

        m_children.push_back(childEntity);

        // Calculate new position for the attached child
        // so it's world space position stays the same
        auto& parentTransform = parentEntity.getComponent<TransformComponent>();
        auto  toGlobalWorld   = glm::inverse(parentTransform.m_worldMatrix) * childTransform.m_worldMatrix;

        glm::vec3 T, R, S;
        math::decompose(toGlobalWorld, T, R, S);

        childTransform.setLocalPosition(T);
        childTransform.setLocalRotation(R);
        childTransform.setLocalScale   (S);
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

        math::decompose(m_worldMatrix, m_localPosition, m_localRotation, m_localScale);
        setLocalRotation(m_localRotation);
    }

    void TransformComponent::update(const glm::mat4& parentTransform, bool dirty)
    {
        dirty |= m_dirty;

        if (dirty)
        {
            m_parentWorldMatrix = parentTransform;
            m_localWorldMatrix  = TRS(m_localPosition, m_localOrientation, m_localScale);
            m_worldMatrix       = m_parentWorldMatrix * m_localWorldMatrix;
            m_normalMatrix      = glm::mat3(glm::transpose(glm::inverse(m_worldMatrix)));

            m_dirty = false;
        }

        for (unsigned i = 0; i < m_children.size(); ++i)
        {
            m_children[i].getComponent<TransformComponent>().update(m_worldMatrix, dirty);
        }
    }

    glm::mat4 TransformComponent::TRS(const glm::vec3& translation, const glm::vec3& euler, const glm::vec3& scale)
    {
        return TRS(translation, glm::quat(euler), scale);
    }

    glm::mat4 TransformComponent::TRS(const glm::vec3& translation, const glm::quat& rotation, const glm::vec3& scale)
    {
        glm::mat4 T = glm::translate(glm::mat4(1.0f), translation);
        glm::mat4 R = glm::mat4_cast(glm::normalize(rotation));
        glm::mat4 S = glm::scale(glm::mat4(1.0f), scale);

        return T * R * S;
    }
}