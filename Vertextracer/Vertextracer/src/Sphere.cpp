#include "Sphere.h"
#include "MathHelpers.h"
#include <glm/gtc/constants.inl>

bool Sphere::intersect(const Ray & ray, float & t) const
{
    float t0, t1; // solutions for t if the ray intersects 

    // analytic solution
    glm::vec3 L = ray.m_origin - m_center;

    float a = glm::dot(ray.m_direction, ray.m_direction);
    float b = 2 * glm::dot(ray.m_direction, L);
    float c = glm::dot(L, L) - m_radius * m_radius;

    if (!solveQuadraticEquation(a, b, c, t0, t1))
    {
        return false;
    }

    if (t0 > t1)
    {
        std::swap(t0, t1);
    }

    if (t0 < 0) 
    {
        t0 = t1; // if t0 is negative, let's use t1 instead 
        if (t0 < 0)
        {
            return false; // both t0 and t1 are negative 
        }
    }

    t = t0;

    return true;
}

void Sphere::getSurfaceData(const glm::vec3 & hit_point, glm::vec3 & normal, glm::vec2 & texcoord) const
{
    normal = hit_point - m_center;
    normal = glm::normalize(normal);
    // In this particular case, the normal is simular to a point on a unit sphere
    // centred around the origin. We can thus use the normal coordinates to compute
    // the spherical coordinates of Phit.
    // atan2 returns a value in the range [-pi, pi] and we need to remap it to range [0, 1]
    // acosf returns a value in the range [0, pi] and we also need to remap it to the range [0, 1]
    texcoord.x = (1 + atan2(normal.z, normal.x) / glm::pi<float>()) * 0.5;
    texcoord.y = acosf(normal.y) / glm::pi<float>();
}
