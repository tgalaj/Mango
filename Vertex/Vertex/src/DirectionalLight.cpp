#include "DirectionalLight.h"


DirectionalLight::DirectionalLight()
    : direction(glm::vec3(1.0)),
      intensity(glm::vec3(1.0f)),
      ambient  (glm::vec3(0.2f)),
      diffuse  (glm::vec3(1.0f)),
      specular (glm::vec3(1.0f))
{
}


DirectionalLight::~DirectionalLight()
{
}
