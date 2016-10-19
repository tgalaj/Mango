#pragma once

#include "Material.h"

struct Sphere
{
    Material mat;

    glm::mat4 transform = glm::mat4(1.0f);
    glm::vec3 center;
    float radius;
};

struct Triangle
{
    Material mat;

    glm::mat4 transform = glm::mat4(1.0f);
    glm::vec3 p0;
    glm::vec3 p1;
    glm::vec3 p2;

    glm::vec3 n;
};


