#pragma once
#include "Ray.h"
#include <random>

class Object
{
public:
    Object()
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0, 1);

        color = glm::vec3(dis(gen), dis(gen), dis(gen));
    }

    virtual ~Object() {}

    virtual bool intersect(const Ray & ray, float & t_near, uint32_t & tri_index, glm::vec2 & texcoord) const = 0;
    virtual void getSurfaceData(const glm::vec3 & hit_point, const glm::vec3 & view_dir, const uint32_t & tri_index, const glm::vec2 & uv, glm::vec3 & hit_normal, glm::vec2 & hit_texcoord) const = 0;

    glm::vec3 color;
};
