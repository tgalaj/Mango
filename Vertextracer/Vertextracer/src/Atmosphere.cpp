#include <glm/gtc/constants.hpp>
#include "Atmosphere.h"
#include "MathHelpers.h"
#include "Framebuffer.h"

const glm::highp_dvec3 Atmosphere::BETA_RAYLEIGH(5.8e-6f, 13.5e-6f, 33.1e-6f);
const glm::highp_dvec3 Atmosphere::BETA_MIE(21e-6f);

glm::highp_dvec3 Atmosphere::computeIncidentLight(const Ray & ray, double t_min, double t_max, glm::highp_dvec3& transmittance)
{
    double t0, t1;
    if (!intersect(ray, t0, t1) || t1 < 0)
    {
        return glm::highp_dvec3(0.0f);
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

    double segment_length = (t_max - t_min) / num_samples;
    double t_current      = t_min;

    /* Rayleight and Mie contributions */
    glm::highp_dvec3 sum_r(0.0f), sum_m(0.0f);
    double optical_depth_r = 0, optical_depth_m = 0;

    /* mu in the paper which is the cosine of the angle between the sun direction and the ray direction */
    double mu = glm::dot(ray.m_direction, -sun_light->m_direction);

    double phase_r = 3.0f / (16.0f * glm::pi<double>()) * (1.0f + mu * mu);
    double g = 0.76f;
    double phase_m = 3.0f / (8.0f * glm::pi<double>()) * ((1.0f - g * g) * (1.0f + mu * mu)) / ((2.0f + g * g) * glm::pow(1.0f + g * g - 2.0f * g * mu, 1.5f));

    for(uint32_t i = 0; i < num_samples; ++i)
    {
        glm::highp_dvec3 sample_position = ray.m_origin + (t_current + segment_length * 0.5f) * ray.m_direction;
        double height = glm::length(sample_position - m_center) - planet_radius;

        /* compute optical depth for view ray */
        double hr = f(-height / h_rayleigh) * segment_length;
        double hm = f(-height / h_mie)      * segment_length;

        optical_depth_r += hr;
        optical_depth_m += hm;

        /* compute optical depth for light ray */
        double t0_light, t1_light;

        Ray light_ray(sample_position, -sun_light->m_direction);
        intersect(light_ray, t0_light, t1_light);

        double segment_length_light = t1_light / num_samples_light;
        double t_current_light = 0;
        double optical_depth_light_r = 0, optical_depth_light_m = 0;

        uint32_t j;
        for(j = 0; j < num_samples_light; ++j)
        {
            glm::highp_dvec3 sample_position_light = sample_position + (t_current_light + segment_length_light * 0.5f) * -sun_light->m_direction;
            double height_light = glm::length(sample_position_light - m_center) - planet_radius;

            if(height_light < 0.0f)
            {
                break;
            }

            optical_depth_light_r += f(-height_light / h_rayleigh) * segment_length_light;
            optical_depth_light_m += f(-height_light / h_mie)      * segment_length_light;
            t_current_light += segment_length_light;
        }

        if(j == num_samples_light)
        {
            glm::highp_dvec3 tau = BETA_RAYLEIGH * (optical_depth_r + optical_depth_light_r) + BETA_MIE * 1.1 * (optical_depth_m + optical_depth_light_m);
            glm::highp_dvec3 attenuation(f(-tau.x), 
                                         f(-tau.y), 
                                         f(-tau.z));

            sum_r += attenuation * hr;
            sum_m += attenuation * hm;
        }

        t_current += segment_length;
    }

    transmittance = sum_r * BETA_RAYLEIGH + sum_m * BETA_MIE;

    return (sum_r * BETA_RAYLEIGH * phase_r + sum_m * BETA_MIE * phase_m) * sun_light->m_intensity;
}

bool Atmosphere::intersect(const Ray & ray, double & t0, double & t1, bool is_planet)
{
    glm::highp_dvec3 L = ray.m_origin - m_center;

    double a = glm::dot(ray.m_direction, ray.m_direction);
    double b = 2 * glm::dot(ray.m_direction, L);

    double radius = is_planet ? planet_radius : atmosphere_radius;
    double c = glm::dot(L, L) - radius * radius;

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

glm::highp_dvec3 Atmosphere::computeColor(const Ray & ray, glm::highp_dvec3& transmittance, double& t_max)
{
    double t0, t1, tMax = std::numeric_limits<double>::max();
    if (intersect(ray, t0, t1, true) && t1 > 0.0f)
    {
        tMax = std::max(0.0, t0);
    }

    if(t_max >= 0)
    {
        tMax = t_max;
    }

    return computeIncidentLight(ray, 0, tMax, transmittance);
}
