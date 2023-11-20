#pragma once
#include "Mango/Core/System.h"
#include "Mango/Scene/Components.h"

namespace mango
{
    class FreePoseSystem : public System
    {
    public:
        FreePoseSystem()
            : m_mousePressedPosition(0.0, 0.0),
              m_isMouseMove         (false)
        {}

        void onUpdate(float dt) override;

    private:
        void move(TransformComponent & transform, const glm::vec3 & dir, float amount);

        glm::vec2 m_mousePressedPosition{};
        bool m_isMouseMove;
    };
}
