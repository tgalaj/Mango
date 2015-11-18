#pragma once
#include "SceneNode.h"

class SpotLight : public SceneNode
{
public:
    SpotLight();
    ~SpotLight();

    glm::vec3 position;
    glm::vec3 direction;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constantAttenuation;
    float linearAttenuation;
    float quadraticAttenuation;

    float innerCutOffAngle;
    float outerCutOffAngle;
};

