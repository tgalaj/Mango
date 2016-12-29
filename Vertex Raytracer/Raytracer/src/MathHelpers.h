#pragma once

#include <cmath>

bool solveQuadraticEquation(float a, float b, float c, float& x1, float& x2)
{
    if(b == 0)
    {
        if(a == 0)
        {
            return false;
        }

        x1 = 0;
        x2 = std::sqrt(-c / a);

        return true;
    }

    float delta = b * b - 4 * a * c;

    if(delta < 0)
    {
        return false;
    }

    float q = (b < 0.0f) ? -0.5f * (b - std::sqrt(delta)) : -0.5f * (b + std::sqrt(delta));

    x1 = q / a;
    x2 = c / q;

    return true;
}