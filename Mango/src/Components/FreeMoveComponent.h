#pragma once

#include "Window/Input.h"

namespace mango
{
    class FreeMoveComponent
    {
    public:
        explicit FreeMoveComponent(float speed = 10.0f)
            : moveSpeed  (speed),
              forwardKey (KeyCode::W),
              backwardKey(KeyCode::S),
              leftKey    (KeyCode::A),
              rightKey   (KeyCode::D), 
              upKey      (KeyCode::E), 
              downKey    (KeyCode::Q)
        {
        }

        float   moveSpeed;
        KeyCode forwardKey;
        KeyCode backwardKey;
        KeyCode leftKey;
        KeyCode rightKey;
        KeyCode upKey;
        KeyCode downKey;
    };
}
