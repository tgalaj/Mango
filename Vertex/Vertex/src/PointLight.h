#pragma once

#include "SceneNode.h"

class PointLight : public SceneNode
{
public:
    PointLight();
    ~PointLight();

    glm::vec3 position;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constantAttenuation;
    float linearAttenuation;
    float quadraticAttenuation;
};

