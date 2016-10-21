#pragma once

#include "Material.h"

enum class PRIMITIVE_TYPE { SPHERE, TRIANGLE, UNDEFINED};

class Primitive
{
public:

    Material mat;
    glm::mat4 transform = glm::mat4(1.0f);
    PRIMITIVE_TYPE type = PRIMITIVE_TYPE::UNDEFINED;
};

class Sphere : public Primitive
{
public:

    Sphere()
    {
        radius = 0.0f;
        type = PRIMITIVE_TYPE::SPHERE;
    }

    glm::vec3 center;
    float radius;
};

class Triangle : public Primitive
{
public:

    Triangle()
    {
        type = PRIMITIVE_TYPE::TRIANGLE;
    }

    glm::vec3 p0;
    glm::vec3 p1;
    glm::vec3 p2;

    glm::vec3 n;
};


