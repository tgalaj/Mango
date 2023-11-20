#pragma once
#include "Mango/Core/System.h"

namespace mango
{
    class CameraSystem : public System
    {
    public:
        void onUpdate(float dt) override;

    };
}
