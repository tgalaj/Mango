#pragma once
#include <glm/glm.hpp>
#include "Scene.h"
#include "Ray.h"

struct Options;

class Raytracer
{
public:
    glm::vec3 castRay(const Ray & primary_ray, const Scene & scene, const Options & options, uint32_t depth);
    bool trace(const Ray & ray, const Scene & scene, float &t_near, uint32_t & index, glm::vec2 & uv, Object **hit_object);
};
