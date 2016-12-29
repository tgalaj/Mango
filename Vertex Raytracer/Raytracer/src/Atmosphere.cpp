#include "Atmosphere.h"

const glm::vec3 Atmosphere::BETA_RAYLEIGH(3.8e-6f, 13.5e-6f, 33.1e-6f);
const glm::vec3 Atmosphere::BETA_MIE(21e-6f);

glm::vec3 Atmosphere::computeIncidentLight(const glm::vec3 & origin, const glm::vec3 & direction, float t_min, float t_max) const
{
    return glm::vec3();
}
