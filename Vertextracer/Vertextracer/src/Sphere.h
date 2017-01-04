#pragma once
#include "Object.h"

class Sphere : public Object
{
public:
    Sphere(const glm::vec3 & center, float radius) 
        : m_center(center), 
          m_radius(radius) {}

    bool intersect(const Ray& ray, float& t) const override;
    void getSurfaceData(const glm::vec3& hit_point, glm::vec3& normal, glm::vec2& texcoord) const override;

    glm::vec3 m_center;
    float m_radius;
};
