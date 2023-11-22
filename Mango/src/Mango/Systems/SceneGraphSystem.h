#pragma once
#include "Mango/Core/System.h"
#include "Mango/Scene/Components.h"

namespace mango
{
    class SceneGraphSystem final : public System
    {
    public:
        SceneGraphSystem();
        ~SceneGraphSystem() = default;

        void onUpdate(float dt) override;

    private:
        static TransformComponent ROOT_NODE;

    private:
        friend class Scene;
    };
}
