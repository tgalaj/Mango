#include "mgpch.h"

#include "CoreSystems/CameraSystem.h"
#include "CoreComponents/CameraComponent.h"
#include "CoreComponents/TransformComponent.h"

namespace mango
{
    void CameraSystem::configure(entityx::EntityManager& entities, entityx::EventManager& events)
    {
    }

    void CameraSystem::update(entityx::EntityManager & entities, entityx::EventManager & events, entityx::TimeDelta dt)
    {
        entities.each<CameraComponent, TransformComponent>(
        [this](entityx::Entity entity, CameraComponent & camera, TransformComponent & transform)
        {
            glm::mat4 R = glm::mat4_cast(transform.orientation());
            glm::mat4 T = glm::translate(glm::mat4(1.0f), -transform.position());

            camera.m_view = R * T;
        });
    }
}
