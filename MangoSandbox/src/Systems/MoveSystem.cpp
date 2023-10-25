#include "MoveSystem.h"
#include <Mango/Components/TransformComponent.h>
#include <Mango/Components/PointLightComponent.h>
#include <Mango/Components/SpotLightComponent.h>

MoveSystem::~MoveSystem()
{
}

void MoveSystem::configure(entityx::EntityManager& entities, entityx::EventManager& events)
{
}

void MoveSystem::update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt)
{
    entityx::ComponentHandle<mango::TransformComponent> transform;
    entityx::ComponentHandle<MoveSystemComponent> msc;
    entityx::ComponentHandle<mango::PointLightComponent> pointLight;
    entityx::ComponentHandle<mango::SpotLightComponent> spotLight;
    
    static float acc = 0.0f;
    acc += dt / 6.0f;

    for(auto entity : entities.entities_with_components(transform, msc, pointLight))
    {
        glm::vec3 delta = transform->getPosition() - glm::vec3(0.0f);

        float r            = 8.0f * glm::abs(glm::sin(acc));
        float currentAngle = atan2(delta.z, delta.x);

        auto position   = transform->getPosition();
             position.x = r * glm::cos(currentAngle + dt);
             position.z = r * glm::sin(currentAngle + dt);

        transform->setPosition(position);
    }

    for(auto entity : entities.entities_with_components(transform, msc, spotLight))
    {
        glm::quat previousOrientation = transform->getOrientation();

        transform->setOrientation(0.0f, 0.025f, 0.0f);
        transform->setOrientation(previousOrientation * transform->getOrientation());
    }
}
