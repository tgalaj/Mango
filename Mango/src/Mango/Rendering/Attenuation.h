#pragma once

namespace mango
{
    class Attenuation
    {
    public:
        Attenuation(float constant = 0.0f, float linear = 0.0f, float quadratic = 1.0f)
            : constant (constant),
              linear   (linear),
              quadratic(quadratic)
        {}

        float constant;
        float linear;
        float quadratic;
    };
}
