#include "Mesh.h"

Mesh::Mesh(const Buffers & buffers)
    : m_buffers(buffers)
{
}

bool Mesh::intersect(const Ray & ray, double & t_near, uint32_t & tri_index, glm::highp_dvec2 & texcoord) const
{
    uint32_t j = 0;
    bool intersect = false;

    static const double INF = std::numeric_limits<double>::max();

    for(uint32_t i = 0; i < m_buffers.m_num_faces; ++i, j += 3)
    {
        const glm::highp_dvec3 & v0 = m_buffers.m_positions[m_buffers.m_indices[j]];
        const glm::highp_dvec3 & v1 = m_buffers.m_positions[m_buffers.m_indices[j + 1]];
        const glm::highp_dvec3 & v2 = m_buffers.m_positions[m_buffers.m_indices[j + 2]];

        double t = INF, u, v;

        if(rayTriangleIntersect(ray, v0, v1, v2, t, u, v) && t < t_near)
        {
            t_near = t;
            texcoord.x = u;
            texcoord.y = v;
            tri_index = i;
            intersect = true;
        }
    }

    return intersect;
}

void Mesh::getSurfaceData(const glm::highp_dvec3 & hit_point, const glm::highp_dvec3 & view_dir, const uint32_t & tri_index, const glm::highp_dvec2 & uv, glm::highp_dvec3 & hit_normal, glm::highp_dvec2 & hit_texcoord, bool flat_shading) const
{
    if (flat_shading)
    {
        // face normal
        const glm::highp_dvec3 &v0 = m_buffers.m_positions[m_buffers.m_indices[tri_index * 3]];
        const glm::highp_dvec3 &v1 = m_buffers.m_positions[m_buffers.m_indices[tri_index * 3 + 1]];
        const glm::highp_dvec3 &v2 = m_buffers.m_positions[m_buffers.m_indices[tri_index * 3 + 2]];
        hit_normal = glm::cross(v1 - v0, v2 - v0);
        hit_normal = glm::normalize(hit_normal);
    }
    else
    {
        // vertex normal
        const glm::highp_dvec3 &n0 = m_buffers.m_normals[m_buffers.m_indices[tri_index * 3]];
        const glm::highp_dvec3 &n1 = m_buffers.m_normals[m_buffers.m_indices[tri_index * 3 + 1]];
        const glm::highp_dvec3 &n2 = m_buffers.m_normals[m_buffers.m_indices[tri_index * 3 + 2]];
        hit_normal = (1 - uv.x - uv.y) * n0 + uv.x * n1 + uv.y * n2;
        hit_normal = glm::normalize(hit_normal);
    }

    // texture coordinates
    const glm::highp_dvec2 &st0 = m_buffers.m_texcoords[m_buffers.m_indices[tri_index * 3]];
    const glm::highp_dvec2 &st1 = m_buffers.m_texcoords[m_buffers.m_indices[tri_index * 3 + 1]];
    const glm::highp_dvec2 &st2 = m_buffers.m_texcoords[m_buffers.m_indices[tri_index * 3 + 2]];
    hit_texcoord = (1 - uv.x - uv.y) * st0 + uv.x * st1 + uv.y * st2;
}

bool Mesh::rayTriangleIntersect(const Ray & ray, const glm::highp_dvec3 & v0, const glm::highp_dvec3 & v1, const glm::highp_dvec3 & v2, double & t, double & u, double & v) const
{
    static const double EPSILON = 1e-8;

    glm::highp_dvec3 edge1 = v1 - v0;
    glm::highp_dvec3 edge2 = v2 - v0;
    glm::highp_dvec3 p_vec = glm::cross(ray.m_direction, edge2);

    double det = glm::dot(edge1, p_vec);

    if(det < EPSILON && det > -EPSILON)
    {
        /* Ray and triangle are parallel*/
        return false;
    }

    double inv_det = 1.0 / det;

    glm::highp_dvec3 t_vec = ray.m_origin - v0;
    u = glm::dot(t_vec, p_vec) * inv_det;

    if(u < 0.0 || u > 1.0)
    {
        return false;
    }

    glm::highp_dvec3 q_vec = glm::cross(t_vec, edge1);
    v = glm::dot(ray.m_direction, q_vec) * inv_det;

    if(v < 0.0 || u + v > 1.0)
    {
        return false;
    }

    t = glm::dot(edge2, q_vec) * inv_det;

    return (t >= 0.0) ? true : false;
}
