#include "Raytracer.h"

glm::vec3 Raytracer::castRay(const Ray & primary_ray, const Scene & scene, uint32_t depth)
{
    return (primary_ray.m_direction + glm::vec3(1.0f)) * 0.5f;
}
