#pragma once

namespace Vertex
{
    class Attenuation
    {
    public:
        Attenuation(float constant = 0.0f, float linear = 0.0f, float quadratic = 1.0f)
            : m_constant (constant),
              m_linear   (linear),
              m_quadratic(quadratic)
        {}

        float m_constant;
        float m_linear;
        float m_quadratic;
    };
}
