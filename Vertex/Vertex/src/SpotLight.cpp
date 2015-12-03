#include "SpotLight.h"


SpotLight::SpotLight()
    : position            (glm::vec3(0.0f)),
      direction           (glm::vec3(1.0f)),
      ambient             (glm::vec3(0.2f)),
      diffuse             (glm::vec3(1.0f)),
      specular            (glm::vec3(1.0f)),
      constantAttenuation (1.0f),
      linearAttenuation   (0.0f),
      quadraticAttenuation(0.0f),
      innerCutOffAngle    (12.5f),
      outerCutOffAngle    (17.5f),
      isDirty             (true)
{
}


SpotLight::~SpotLight()
{
}
