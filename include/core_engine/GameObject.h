#pragma once
#include <entityx/Entity.h>

namespace Vertex
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

    protected:
        entityx::Entity entity;
    };
}
