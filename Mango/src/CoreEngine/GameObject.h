#pragma once
#include "entityx/Entity.h"
#include "CoreComponents/TransformComponent.h"

namespace mango
{
    class GameObject
    {
    public:
        GameObject();

        template <typename S, typename ... Args>
        void addComponent(Args && ... args)
        {
            entity.assign<S>(std::forward<Args>(args) ...);
        }

        template <typename C>
        entityx::ComponentHandle<C> getComponent()
        {
            return entity.component<C>();
        }

        virtual ~GameObject() {}

        void setPosition(float x, float y, float z);
        void setPosition(const glm::vec3 & position);
        void setOrientation(float x, float y, float z);
        void setOrientation(const glm::vec3 & axis, float angle);
        void setOrientation(const glm::quat & quat);
        void setScale(float x, float y, float z);
        void setScale(float uniform_scale);
        void addChild(GameObject & child);

    protected:
        entityx::Entity entity;
    };
}
