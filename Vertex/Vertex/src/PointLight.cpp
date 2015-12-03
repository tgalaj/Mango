#include "PointLight.h"


PointLight::PointLight()
    : position            (glm::vec3(0.0f)),
      ambient             (glm::vec3(0.2f)),
      diffuse             (glm::vec3(1.0f)),
      specular            (glm::vec3(1.0f)),
      constantAttenuation (1.0f),
      linearAttenuation   (0.0f),
      quadraticAttenuation(0.0f),
      isDirty             (true)
    
{
}


PointLight::~PointLight()
{
}
