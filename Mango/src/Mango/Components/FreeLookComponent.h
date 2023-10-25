#pragma once

#include "Mango/Window/Input.h"

namespace mango
{
    class FreeLookComponent
    {
    public:
        explicit FreeLookComponent(float sensitivity = 0.2f)
            : sensitivity(sensitivity),
              unlockMouseKey(KeyCode::MouseRight) {}

        float   sensitivity;
        KeyCode unlockMouseKey;
    };
}
