#pragma once

#define GLM_ENABLE_EXPERIMENTAL

#include <entityx/Entity.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/vec3.hpp>
#include <vector>

namespace mango
{
    class TransformComponent
    {
    public:
        TransformComponent(const glm::vec3 & position    = glm::vec3(0.0f),
                           const glm::vec3 & orientation = glm::vec3(0.0f, 0.0f, 0.0f),
                           const glm::vec3 & scale       = glm::vec3(1.0f))
            : m_orientation (orientation),
              m_position    (position),
              m_scale       (scale),
              m_isDirty     (true),
              m_worldMatrix (glm::mat4(1.0f)),
              m_normalMatrix(glm::mat3(1.0f)),
              m_direction   (glm::vec3(0.0f, 0.0f, -1.0f))
        {}

        void setPosition(float x, float y, float z)
        {
            m_position = glm::vec3(x, y, z);
            m_isDirty  = true;
        }

        void setPosition(const glm::vec3 & position)
        {
            m_position = position;
            m_isDirty  = true;
        }

        /*
         * Set orientation using Euler Angles in degrees
         */
        
        void setOrientation(float x, float y, float z)
        {
            m_orientation = glm::angleAxis(glm::radians(x), glm::vec3(1.0f, 0.0f, 0.0f)) *
                            glm::angleAxis(glm::radians(y), glm::vec3(0.0f, 1.0f, 0.0f)) *
                            glm::angleAxis(glm::radians(z), glm::vec3(0.0f, 0.0f, 1.0f));
            m_direction   = glm::normalize(glm::conjugate(m_orientation) * glm::vec3(0.0f, 0.0f, 1.0f));
            m_isDirty     = true;
        }

        /*
        * Set orientation using axis and angle in degrees
        */
        void setOrientation(const glm::vec3 & axis, float angle)
        {
            m_orientation = glm::angleAxis(glm::radians(angle), glm::normalize(axis));
            m_direction   = glm::normalize(glm::conjugate(m_orientation) * glm::vec3(0.0f, 0.0f, 1.0f));
            m_isDirty    = true;
        }

        void setOrientation(const glm::quat & quat)
        {
            m_orientation = quat;
            m_direction   = glm::normalize(glm::conjugate(m_orientation) * glm::vec3(0.0f, 0.0f, 1.0f));
            m_isDirty     = true;
        }

        void setScale(float x, float y, float z)
        {
            m_scale   = glm::vec3(x, y, z);
            m_isDirty = true;
        }

        void setScale(float uniformScale)
        {
            m_scale   = glm::vec3(uniformScale);
            m_isDirty = true;
        }

        void addChild(const entityx::ComponentHandle<TransformComponent> & child)
        {
            m_children.push_back(child);
        }

        void update(const glm::mat4 & parentTransform, bool dirty)
        {
            dirty |= m_isDirty;

            if(dirty)
            {
                m_worldMatrix  = getUpdatedWorldMatrix();
                m_worldMatrix  = parentTransform * m_worldMatrix;
                m_normalMatrix = glm::mat3(glm::transpose(glm::inverse(m_worldMatrix)));

                m_isDirty = false;
            }

            for(unsigned i = 0; i < m_children.size(); ++i)
            {
                m_children[i]->update(m_worldMatrix, dirty);
            }
        }

        glm::mat4 getWorldMatrix () const { return m_worldMatrix;  }
        glm::mat3 getNormalMatrix() const { return m_normalMatrix; }
        glm::quat getOrientation () const { return m_orientation;  }
        glm::vec3 getPosition    () const { return m_position;     }
        glm::vec3 getScale       () const { return m_scale;        }
        glm::vec3 getDirection   () const { return m_direction;    }

    private:
        glm::mat4 getUpdatedWorldMatrix() const
        {
            glm::mat4 T = glm::translate(glm::mat4(1.0f), m_position);
            glm::mat4 R = glm::mat4_cast(m_orientation);
            glm::mat4 S = glm::scale(glm::mat4(1.0f), m_scale);

            return T * R * S;
        }

    private:
        std::vector<entityx::ComponentHandle<TransformComponent>> m_children;

        glm::mat4 m_worldMatrix{};
        glm::mat3 m_normalMatrix{};

        glm::quat m_orientation{};
        glm::vec3 m_position{};
        glm::vec3 m_scale{};
        glm::vec3 m_direction{};
        
        bool m_isDirty;
    };
}
