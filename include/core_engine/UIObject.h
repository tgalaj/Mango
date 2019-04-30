#pragma once
#include <entityx/Entity.h>

namespace Vertex
{
    class UIObject
    {
    public:
        UIObject();
        virtual ~UIObject() {}

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

    protected:
        entityx::Entity entity;
    };
}
