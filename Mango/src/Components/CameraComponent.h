#pragma once

#include <glm/gtc/matrix_transform.hpp>

namespace mango
{
    class CameraComponent
    {
    public:
        CameraComponent()
            : isOrtho(false)
        {
        }

        /*
         * Perspective camera
         */
        CameraComponent(float fov, 
                        float aspectRatio, 
                        float zNear, 
                        float zFar)
            : view      (glm::mat4(1.0f)),
              projection(glm::perspective(glm::radians(fov), aspectRatio, zNear, zFar)), 
              isOrtho   (false)
        {
        }

        /*
         * Ortho camera
         */
        CameraComponent(float left, 
                        float right, 
                        float bottom, 
                        float top,
                        float zNear, 
                        float zFar)
            : view      (glm::mat4(1.0f)),
              projection(glm::ortho(left, right, bottom, top, zNear, zFar)),
              isOrtho   (true)
        {
        }

        glm::mat4  view{};
        glm::mat4  projection{};
        const bool isOrtho;
    };
}
