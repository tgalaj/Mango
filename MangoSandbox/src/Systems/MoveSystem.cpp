#include "MoveSystem.h"
#include "CoreComponents/TransformComponent.h"
#include "CoreComponents/PointLightComponent.h"
#include "CoreComponents/SpotLightComponent.h"

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
    entityx::ComponentHandle<mango::PointLightComponent> point_light;
    entityx::ComponentHandle<mango::SpotLightComponent> spot_light;
    
    static float acc = 0.0f;
    acc += dt / 6.0f;

    for(auto entity : entities.entities_with_components(transform, msc, point_light))
    {
        glm::vec3 delta = transform->position() - glm::vec3(0.0f);

        float r = 8.0f * glm::abs(glm::sin(acc));
        float current_angle = atan2(delta.z, delta.x);

        auto position = transform->position();
        position.x = r * glm::cos(current_angle + dt);
        position.z = r * glm::sin(current_angle + dt);

        transform->setPosition(position);
    }

    for(auto entity : entities.entities_with_components(transform, msc, spot_light))
    {
        glm::quat previous_orientation = transform->orientation();

        transform->setOrientation(0.0f, 0.025f, 0.0f);
        transform->setOrientation(previous_orientation * transform->orientation());
    }
}
