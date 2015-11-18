#pragma once

#include <glm\glm.hpp>

class Material
{
public:
    Material();
    ~Material();

    static unsigned int ID;

    glm::vec3 diffuse_color;
    float     shininess;
};

