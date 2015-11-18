#pragma once

#include "SceneNode.h"

class DirectionalLight : public SceneNode
{
public:
    DirectionalLight();
    ~DirectionalLight();

    glm::vec3 direction;
    glm::vec3 intensity;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

