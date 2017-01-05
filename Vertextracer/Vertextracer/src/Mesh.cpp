#include "Mesh.h"

bool Mesh::intersect(const Ray & ray, float & t) const
{
    return false;
}

void Mesh::getSurfaceData(const glm::vec3 & hit_point, glm::vec3 & normal, glm::vec2 & texcoord) const
{
}

bool Mesh::rayTriangleIntersect(const Ray & ray, const glm::vec3 & v0, const glm::vec3 & v1, const glm::vec3 & v2, float & t, float & u, float & v)
{
    static const float EPSILON = 1e-8;

    glm::vec3 edge1 = v1 - v0;
    glm::vec3 edge2 = v2 - v0;
    glm::vec3 p_vec = glm::cross(ray.m_direction, edge2);

    float det = glm::dot(edge1, p_vec);

    if(fabs(det) < EPSILON)
    {
        /* Ray and triangle are parallel*/
        return false;
    }

    float inv_det = 1.0f / det;

    glm::vec3 t_vec = ray.m_origin - v0;
    u = glm::dot(t_vec, p_vec) * inv_det;

    if(u < 0.0f || u > 1.0f)
    {
        return false;
    }

    glm::vec3 q_vec = glm::cross(t_vec, edge1);
    v = glm::dot(ray.m_direction, q_vec) * inv_det;

    if(v < 0.0f || u + v > 1.0f)
    {
        return false;
    }

    t = glm::dot(edge2, q_vec) * inv_det;

    return true;
}
