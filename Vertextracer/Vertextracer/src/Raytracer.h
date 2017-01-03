#pragma once
#include <glm/glm.hpp>
#include "Scene.h"
#include "Ray.h"

class Raytracer
{
public:
    glm::vec3 castRay(const Ray & primary_ray, const Scene & scene, uint32_t depth);
};
