#include <utility>

#include "Ray.h"
#include "MathHelpers.h"

Ray::Ray(const glm::vec3 & origin, const glm::vec3 & direction)
{
    this->origin = origin;
    this->direction = direction;
}


Ray::~Ray()
{
}

bool Ray::checkIntersection(const Sphere & s, float & t0, float & t1) const
{
    glm::mat4 T  = glm::inverse(s.transform);

    glm::vec3 r_dir    = glm::vec3(T * glm::vec4(direction, 0.0f));
    glm::vec3 r_origin = glm::vec3(T * glm::vec4(origin, 1.0f));

    float a = glm::dot(r_dir, r_dir);
    float b = 2.0f * glm::dot(r_dir, r_origin - s.center);
    float c = glm::dot(r_origin - s.center, r_origin - s.center) - s.radius * s.radius;

    if(!solveQuadraticEquation(a, b, c, t0, t1))
    {
        return false;
    }

    if(t0 > t1) std::swap(t0, t1);

    return true;
}

bool Ray::checkIntersection(const Triangle & t, float & d) const
{
    d = -888.0f;
    glm::mat4 T  = glm::inverse(t.transform);

    glm::vec3 r_dir    = glm::vec3(T * glm::vec4(direction, 0.0f));
    glm::vec3 r_origin = glm::vec3(T * glm::vec4(origin, 1.0f));

    float eps = 0.000001f;

    glm::vec3 e1 = t.p1 - t.p0;
    glm::vec3 e2 = t.p2 - t.p0;

    /* Begin calculating determinant */
    glm::vec3 p = glm::cross(r_dir, e2);

    /* If determinant is near zero, ray lies in plane of a triangle */
    float det = glm::dot(e1, p);

    /* Not culling */
    if (det > -eps && det < eps)
        return false;

    float inv_det = 1.0f / det;

    /* Calculate distance from V0 to ray origin */
    glm::vec3 tt = r_origin - t.p0;

    /* Calculate u parameter and test bound */
    float u = glm::dot(tt, p) * inv_det;

    /* Check if intersection lies outside of the triangle */
    if (u < 0.0f || u > 1.0f)
        return false;

    /* Prepare to test v parameter */
    glm::vec3 q = glm::cross(tt, e1);
    float v = glm::dot(r_dir, q) * inv_det;

    /* Check if intersection lies outside of the triangle */
    if (v < 0.0f || u + v > 1.0f)
        return false;

    float dd = glm::dot(e2, q) * inv_det;

    /* Ray intersection */
    if (dd > eps)
    {
        d = dd;
        return true;
    }

    return false;
}
