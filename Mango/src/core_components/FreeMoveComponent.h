#pragma once
#include "framework/window/Input.h"

namespace mango
{
    class FreeMoveComponent
    {
    public:
        explicit FreeMoveComponent(float speed = 10.0f)
            : m_move_speed(speed),
              m_forward_key(KeyCode::W),
              m_backward_key(KeyCode::S),
              m_left_key(KeyCode::A),
              m_right_key(KeyCode::D), 
              m_up_key(KeyCode::E), 
              m_down_key(KeyCode::Q)
        {
        }

        float m_move_speed;
        KeyCode m_forward_key;
        KeyCode m_backward_key;
        KeyCode m_left_key;
        KeyCode m_right_key;
        KeyCode m_up_key;
        KeyCode m_down_key;
    };
}
