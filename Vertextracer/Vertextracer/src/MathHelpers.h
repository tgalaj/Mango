#pragma once

#include <cmath>
#include <algorithm>

static bool solveQuadraticEquation(double a, double b, double c, double& x1, double& x2)
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

    double delta = b * b - 4 * a * c;

    if(delta < 0)
    {
        return false;
    }

    double q = (b < 0.0f) ? -0.5f * (b - std::sqrt(delta)) : -0.5f * (b + std::sqrt(delta));

    x1 = q / a;
    x2 = c / q;

    //if(x1 > x2)
    //{
    //    std::swap(x1, x2);
    //}

    return true;
}