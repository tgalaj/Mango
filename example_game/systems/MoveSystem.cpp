#include "MoveSystem.h"
#include "core_components/TransformComponent.h"

MoveSystem::~MoveSystem()
{
}

void MoveSystem::configure(entityx::EntityManager& entities, entityx::EventManager& events)
{
}

void MoveSystem::update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt)
{
    entityx::ComponentHandle<Vertex::TransformComponent> transform;
    entityx::ComponentHandle<MoveSystemComponent> msc;
    
    static float acc = 0.0f;
    acc += dt / 6.0f;

    for(auto entity : entities.entities_with_components(transform, msc))
    {
        glm::vec3 delta = transform->position() - glm::vec3(0.0f);

        float r = 8.0f * glm::abs(glm::sin(acc));
        float current_angle = atan2(delta.z, delta.x);

        auto position = transform->position();
        position.x = r * glm::cos(current_angle + dt);
        position.z = r * glm::sin(current_angle + dt);

        //transform->setPosition(position);
    }
}
