#pragma once
#include "Object.h"

class Mesh : public Object
{
public:

    bool intersect(const Ray& ray, float& t) const override;
    void getSurfaceData(const glm::vec3& hit_point, glm::vec3& normal, glm::vec2& texcoord) const override;

private:
    /* Moller-Trumbore ray triangle intersection algorithm */
    bool rayTriangleIntersect(const Ray & ray, const glm::vec3 & v0, const glm::vec3 & v1, const glm::vec3 & v2, float & t, float & u, float & v);
};
