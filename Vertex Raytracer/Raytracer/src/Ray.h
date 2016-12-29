#pragma once

#include <glm/glm.hpp>
#include "Primitives.h"

class Ray
{
public:
    Ray(const glm::vec3 & origin, const glm::vec3 & direction);
    ~Ray();

    bool checkIntersection(const Sphere & s, float & t0, float & t1) const;
    bool checkIntersection(const Triangle & t, float & d) const;

    glm::vec3 origin;
    glm::vec3 direction;
};

