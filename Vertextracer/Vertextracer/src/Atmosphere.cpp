#include <glm/gtc/constants.hpp>
#include "Atmosphere.h"
#include "MathHelpers.h"

const glm::vec3 Atmosphere::BETA_RAYLEIGH(3.8e-6f, 13.5e-6f, 33.1e-6f);
const glm::vec3 Atmosphere::BETA_MIE(21e-6f);

glm::vec3 Atmosphere::computeIncidentLight(const Ray & ray, float t_min, float t_max)
{
    float t0, t1;
    if (!intersect(ray, t0, t1) || t1 < 0)
    {
        return glm::vec3(0.0f);
    }

    if(t0 > t_min && t0 > 0.0f)
    {
        t_min = t0;
    }

    if(t1 < t_max)
    {
        t_max = t1;
    }

    uint32_t num_samples       = 16;
    uint32_t num_samples_light = 8;

    float segment_length = (t_max - t_min) / num_samples;
    float t_current      = t_min;

    /* Rayleight and Mie contributions */
    glm::vec3 sum_r(0.0f), sum_m(0.0f);
    float optical_depth_r = 0, optical_depth_m = 0;

    /* mu in the paper which is the cosine of the angle between the sun direction and the ray direction */
    float mu = glm::dot(ray.m_direction, sun_direction);

    float phase_r = 3.0f / (16.0f * glm::pi<float>()) * (1.0f + mu * mu);
    float g = 0.76f;
    float phase_m = 3.0f / (8.0f * glm::pi<float>()) * ((1.0f - g * g) * (1.0f + mu * mu)) / ((2.0f + g * g) * glm::pow(1.0f + g * g - 2.0f * g * mu, 1.5f));

    for(uint32_t i = 0; i < num_samples; ++i)
    {
        glm::vec3 sample_position = ray.m_origin + (t_current + segment_length * 0.5f) * ray.m_direction;
        float height = glm::length(sample_position) - planet_radius;

        /* compute optical depth for view ray */
        float hr = glm::exp(-height / h_rayleigh) * segment_length;
        float hm = glm::exp(-height / h_mie)      * segment_length;

        optical_depth_r += hr;
        optical_depth_m += hm;

        /* compute optical depth for light ray */
        float t0_light, t1_light;

        Ray light_ray(sample_position, sun_direction);
        intersect(light_ray, t0_light, t1_light);

        float segment_length_light = t1_light / num_samples_light;
        float t_current_light = 0;
        float optical_depth_light_r = 0, optical_depth_light_m = 0;

        uint32_t j;
        for(j = 0; j < num_samples_light; ++j)
        {
            glm::vec3 sample_position_light = sample_position + (t_current_light + segment_length_light * 0.5f) * sun_direction;
            float height_light = glm::length(sample_position_light) - planet_radius;

            if(height_light < 0.0f)
            {
                break;
            }

            optical_depth_light_r += glm::exp(-height_light / h_rayleigh) * segment_length_light;
            optical_depth_light_m += glm::exp(-height_light / h_mie)      * segment_length_light;
            t_current_light += segment_length_light;
        }

        if(j == num_samples_light)
        {
            glm::vec3 tau = BETA_RAYLEIGH * (optical_depth_r + optical_depth_light_r) + BETA_MIE * 1.1f * (optical_depth_m + optical_depth_light_m);
            glm::vec3 attenuation(glm::exp(-tau.x), glm::exp(-tau.y), glm::exp(-tau.z));

            sum_r += attenuation * hr;
            sum_m += attenuation * hm;
        }

        t_current += segment_length;
    }

    static float sun_intensity = 20.0f;
    return (sum_r * BETA_RAYLEIGH * phase_r + sum_m * BETA_MIE * phase_m) * sun_intensity;
}

bool Atmosphere::intersect(const Ray & ray, float & t0, float & t1, bool is_planet)
{
    float a = glm::dot(ray.m_direction, ray.m_direction);
    float b = 2 * glm::dot(ray.m_direction, ray.m_origin);

    float radius = is_planet ? planet_radius : atmosphere_radius;
    float c = glm::dot(ray.m_origin, ray.m_origin) - radius * radius;

    if(!solveQuadraticEquation(a, b, c, t0, t1))
    {
        return false;
    }

    //if(t0 < 0.0f)
    //{
    //    t0 = t1;

    //    if(t0 < 0.0f)
    //    {
    //        return false; //both t0 and t1 are negative
    //    }
    //}

    if(t0 > t1)
    {
        std::swap(t0, t1);
    }

    return true;
}
