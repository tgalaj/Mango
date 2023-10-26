#pragma once

#include <entt.hpp>

namespace mango
{
    class Scene
    {
    public:
        Scene();
        ~Scene();

    private:
        entt::registry m_registry;
    };
}

