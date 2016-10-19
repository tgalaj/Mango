#pragma once

#include <glm/glm.hpp>

enum LIGH_TYPE { DIRECTIONAL, POINT };

class Light
{
public:
    Light()
    {
        light_intensity = 1.0f;
        isVisible = true;
    }

    ~Light() {};

    glm::mat4 transform;

    glm::vec3 color;
    glm::vec3 dir_pos;
    glm::vec3 attenuation;

    float light_intensity;
    bool isVisible;

    LIGH_TYPE type;
};

