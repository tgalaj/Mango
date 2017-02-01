#pragma once
#include <glm/glm.hpp>
#include "Scene.h"
#include "Ray.h"

enum class RayType { PRIMARY, SHADOW };

class Raytracer
{
public:
    glm::highp_dvec3 castRay(const Ray & primary_ray, const Scene & scene, const Options & options, const uint32_t & depth = 0);

private:
    struct IntersectInfo
    {
        const Object *hitObject = nullptr;
        double tNear = std::numeric_limits<double>::max();
        glm::highp_dvec2 uv;
        uint32_t triangle_index = 0;
        uint32_t parent_index = 0;
    };

    bool trace(const Ray & ray, const Scene & scene, IntersectInfo & intersect_info, RayType ray_type = RayType::PRIMARY);
    glm::highp_dvec3 traceAtmosphere(const Ray & ray, const Scene & scene, const Options & options);
    glm::highp_dvec3 traceAtmosphere(const Ray & ray, const Scene & scene, const Options & options, glm::highp_dvec3& transmittance, double& t_max);
    glm::highp_dvec3 refract(const glm::highp_dvec3 & I, const glm::highp_dvec3 & N, const double & ior) const;
    void fresnel(const glm::highp_dvec3 & I, const glm::highp_dvec3 & N, const double & ior, double & kr);
};
