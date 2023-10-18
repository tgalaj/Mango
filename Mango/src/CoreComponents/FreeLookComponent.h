#pragma once
#include "Window/Input.h"

namespace mango
{
    class FreeLookComponent
    {
    public:
        explicit FreeLookComponent(float sensitivity = 0.2f)
            : m_sensitivity(sensitivity),
              m_unlock_mouse_key(KeyCode::MouseRight) {}

        float m_sensitivity;
        KeyCode m_unlock_mouse_key;
    };
}
