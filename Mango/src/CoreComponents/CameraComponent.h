#pragma once
#include <glm/gtc/matrix_transform.hpp>

namespace mango
{
    class CameraComponent
    {
    public:
        CameraComponent()
            : m_is_ortho(false)
        {
        }

        /*
         * Perspective camera
         */
        CameraComponent(float fov, 
                        float aspect_ratio, 
                        float z_near, 
                        float z_far)
            : m_view(glm::mat4(1.0f)),
              m_projection(glm::perspective(glm::radians(fov), aspect_ratio, z_near, z_far)), 
              m_is_ortho(false)
        {
        }

        /*
         * Ortho camera
         */
        CameraComponent(float left, 
                        float right, 
                        float bottom, 
                        float top,
                        float z_near, 
                        float z_far)
            : m_view(glm::mat4(1.0f)),
              m_projection(glm::ortho(left, right, bottom, top, z_near, z_far)),
              m_is_ortho(true)
        {
        }

        glm::mat4 m_view;
        glm::mat4 m_projection;
        const bool m_is_ortho;
    };
}
