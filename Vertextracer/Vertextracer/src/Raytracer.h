#pragma once
#include <glm/glm.hpp>
#include "Scene.h"
#include "Ray.h"
#include "DirectionalLight.h"

struct Options;

enum class RayType { PRIMARY, SHADOW };

class Raytracer
{
public:
    glm::vec3 castRay(const Ray & primary_ray, const Scene & scene, const Options & options, const uint32_t & depth = 0);

private:
    struct IntersectInfo
    {
        const Object *hitObject = nullptr;
        float tNear = std::numeric_limits<float>::max();
        glm::vec2 uv;
        uint32_t triangle_index = 0;
        uint32_t parent_index = 0;
    };

    bool trace(const Ray & ray, const Scene & scene, IntersectInfo & intersect_info, RayType ray_type = RayType::PRIMARY);
    glm::vec3 refract(const glm::vec3 & I, const glm::vec3 & N, const float & ior) const;
    void fresnel(const glm::vec3 & I, const glm::vec3 & N, const float & ior, float & kr);
};
