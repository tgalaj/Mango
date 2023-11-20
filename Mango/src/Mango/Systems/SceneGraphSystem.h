#pragma once
#include "Mango/Core/System.h"
#include "Mango/Scene/Components.h"

namespace mango
{
    class SceneGraphSystem : public System
    {
    public:
        void onUpdate(float dt) override;

    private:
        static TransformComponent ROOT_NODE;

    private:
        friend class Scene;
    };
}
