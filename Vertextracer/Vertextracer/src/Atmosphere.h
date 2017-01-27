#pragma once

#include <glm/glm.hpp>
#include "Ray.h"

class Atmosphere
{
public:
    explicit Atmosphere(glm::vec3 _sun_direction = glm::vec3(0, 1, 0), 
                        float _earth_radius      = 6360e3f,
                        float _atmosphere_radius = 6420e3f,
                        float _h_rayleigh        = 7994.0f,
                        float _h_mie             = 1200.0f)
                        : sun_direction    (glm::normalize(_sun_direction)),
                          planet_radius    (_earth_radius),
                          atmosphere_radius(_atmosphere_radius),
                          h_rayleigh       (_h_rayleigh),
                          h_mie            (_h_mie)
    {
        m_center = glm::vec3(0.0f, 
                             planet_radius + 1000.0f, 
                             0.0f);
    }

    ~Atmosphere() {}

    glm::vec3 computeIncidentLight(const Ray & ray, float t_min, float t_max, glm::vec3& transmittance);
    bool intersect(const Ray & ray, float & t0, float & t1, bool is_planet = false);
    glm::vec3 computeColor(const Ray & ray, glm::vec3& transmittance, float& t_max);

    glm::vec3 sun_direction; //Needs to be normalized
    glm::vec3 m_center;
    float planet_radius;
    float atmosphere_radius;
    float h_rayleigh;
    float h_mie;

    static const glm::vec3 BETA_RAYLEIGH;
    static const glm::vec3 BETA_MIE;
};
