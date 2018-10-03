#pragma once
#define GLM_ENABLE_EXPERIMENTAL

#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <entityx/Entity.h>

#include <vector>

namespace Vertex
{
    class TransformComponent
    {
    public:

        explicit TransformComponent(const glm::vec3 & position    = glm::vec3(0.0f), 
                                    const glm::vec3 & orientation = glm::vec3(0.0f, 0.0f, 0.0f),
                                    const glm::vec3 & scale       = glm::vec3(1.0f))
            : m_orientation(orientation),
              m_position   (position),
              m_scale      (scale),
              m_is_dirty   (true)
        {
        }

        void setPosition(float x, float y, float z)
        {
            m_position = glm::vec3(x, y, z);
            m_is_dirty = true;
        }

        void setPosition(const glm::vec3 & position)
        {
            m_position = position;
            m_is_dirty = true;
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
            m_is_dirty    = true;
        }

        /*
        * Set orientation using axis and angle in degrees
        */
        void setOrientation(const glm::vec3 & axis, float angle)
        {
            m_orientation = glm::angleAxis(glm::radians(angle), glm::normalize(axis));
            m_direction   = glm::normalize(glm::conjugate(m_orientation) * glm::vec3(0.0f, 0.0f, 1.0f));
            m_is_dirty    = true;
        }

        void setOrientation(const glm::quat & quat)
        {
            m_orientation = quat;
            m_direction   = glm::normalize(glm::conjugate(m_orientation) * glm::vec3(0.0f, 0.0f, 1.0f));
            m_is_dirty    = true;
        }

        void setScale(float x, float y, float z)
        {
            m_scale    = glm::vec3(x, y, z);
            m_is_dirty = true;
        }

        void setScale(float uniform_scale)
        {
            m_scale    = glm::vec3(uniform_scale);
            m_is_dirty = true;
        }

        void addChild(const entityx::ComponentHandle<TransformComponent> & child)
        {
            m_children.push_back(child);
        }

        void update(const glm::mat4 & parent_transform, bool dirty)
        {
            dirty |= m_is_dirty;

            if(dirty)
            {
                m_world_matrix  = getUpdatedWorldMatrix();
                m_world_matrix  = parent_transform * m_world_matrix;
                m_normal_matrix = glm::mat3(glm::transpose(glm::inverse(m_world_matrix)));

                m_is_dirty = false;
            }

            for(unsigned i = 0; i < m_children.size(); ++i)
            {
                m_children[i]->update(m_world_matrix, dirty);
            }
        }

        glm::mat4 world_matrix()  const { return m_world_matrix;  }
        glm::mat3 normal_matrix() const { return m_normal_matrix; }
        glm::quat orientation()   const { return m_orientation;   }
        glm::vec3 position()      const { return m_position;      }
        glm::vec3 scale()         const { return m_scale;         }
        glm::vec3 direction()     const { return m_direction;     }

    private:
        glm::mat4 getUpdatedWorldMatrix() const
        {
            glm::mat4 T = glm::translate(glm::mat4(1.0f), m_position);
            glm::mat4 R = glm::mat4_cast(m_orientation);
            glm::mat4 S = glm::scale(glm::mat4(1.0f), m_scale);

            return T * R * S;
        }

        std::vector<entityx::ComponentHandle<TransformComponent>> m_children;

        glm::mat4 m_world_matrix;
        glm::mat3 m_normal_matrix;

        glm::quat m_orientation;
        glm::vec3 m_position;
        glm::vec3 m_scale;
        glm::vec3 m_direction;

        bool m_is_dirty;
    };
}
