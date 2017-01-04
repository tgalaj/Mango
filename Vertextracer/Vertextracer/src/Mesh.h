#pragma once
#include "Object.h"

class Mesh : public Object
{
public:

    bool intersect(const Ray& ray, float& t) const override;
    void getSurfaceData(const glm::vec3& hit_point, glm::vec3& normal, glm::vec2& texcoord) const override;
};
