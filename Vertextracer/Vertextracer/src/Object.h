#pragma once
#include "Ray.h"

class Object
{
public:
    Object()
    {
    }

    virtual ~Object() {}

    virtual bool intersect(const Ray & ray, float & t_near, uint32_t & tri_index, glm::vec2 & texcoord) const = 0;
    virtual void getSurfaceData(const glm::vec3 & hit_point, const glm::vec3 & view_dir, const uint32_t & tri_index, const glm::vec2 & uv, glm::vec3 & hit_normal, glm::vec2 & hit_texcoord, bool flat_shading) const = 0;
};
