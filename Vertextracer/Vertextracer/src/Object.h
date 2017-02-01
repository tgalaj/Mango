#pragma once
#include "Ray.h"

class Object
{
public:
    Object()
    {
    }

    virtual ~Object() {}

    virtual bool intersect(const Ray & ray, double & t_near, uint32_t & tri_index, glm::highp_dvec2 & texcoord) const = 0;
    virtual void getSurfaceData(const glm::highp_dvec3 & hit_point, const glm::highp_dvec3 & view_dir, const uint32_t & tri_index, const glm::highp_dvec2 & uv, glm::highp_dvec3 & hit_normal, glm::highp_dvec2 & hit_texcoord, bool flat_shading) const = 0;
};
