#include "mgpch.h"
#include "CameraSystem.h"

namespace mango
{
    void CameraSystem::onUpdate(float dt)
    {
        //entities.each<CameraComponent, TransformComponent>(
        //[this](entityx::Entity entity, CameraComponent & camera, TransformComponent & transform)
        //{
        //    glm::mat4 R = glm::mat4_cast(transform.getOrientation());
        //    glm::mat4 T = glm::translate(glm::mat4(1.0f), -transform.getPosition());

        //    camera.view = R * T;
        //});
    }
}
