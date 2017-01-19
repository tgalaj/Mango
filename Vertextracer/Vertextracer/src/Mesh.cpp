#include "Mesh.h"

Mesh::Mesh(const Buffers & buffers)
    : m_buffers(buffers)
{
}

bool Mesh::intersect(const Ray & ray, float & t_near, uint32_t & tri_index, glm::vec2 & texcoord) const
{
    uint32_t j = 0;
    bool intersect = false;

    static const float INF = std::numeric_limits<float>::max();

    for(uint32_t i = 0; i < m_buffers.m_num_faces; ++i, j += 3)
    {
        const glm::vec3 & v0 = m_buffers.m_positions[m_buffers.m_indices[j]];
        const glm::vec3 & v1 = m_buffers.m_positions[m_buffers.m_indices[j + 1]];
        const glm::vec3 & v2 = m_buffers.m_positions[m_buffers.m_indices[j + 2]];

        float t = INF, u, v;

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

void Mesh::getSurfaceData(const glm::vec3 & hit_point, const glm::vec3 & view_dir, const uint32_t & tri_index, const glm::vec2 & uv, glm::vec3 & hit_normal, glm::vec2 & hit_texcoord, bool flat_shading) const
{
    if (flat_shading)
    {
        // face normal
        const glm::vec3 &v0 = m_buffers.m_positions[m_buffers.m_indices[tri_index * 3]];
        const glm::vec3 &v1 = m_buffers.m_positions[m_buffers.m_indices[tri_index * 3 + 1]];
        const glm::vec3 &v2 = m_buffers.m_positions[m_buffers.m_indices[tri_index * 3 + 2]];
        hit_normal = glm::cross(v1 - v0, v2 - v0);
        hit_normal = glm::normalize(hit_normal);
    }
    else
    {
        // vertex normal
        const glm::vec3 &n0 = m_buffers.m_normals[m_buffers.m_indices[tri_index * 3]];
        const glm::vec3 &n1 = m_buffers.m_normals[m_buffers.m_indices[tri_index * 3 + 1]];
        const glm::vec3 &n2 = m_buffers.m_normals[m_buffers.m_indices[tri_index * 3 + 2]];
        hit_normal = (1 - uv.x - uv.y) * n0 + uv.x * n1 + uv.y * n2;
        hit_normal = glm::normalize(hit_normal);
    }

    // texture coordinates
    const glm::vec2 &st0 = m_buffers.m_texcoords[m_buffers.m_indices[tri_index * 3]];
    const glm::vec2 &st1 = m_buffers.m_texcoords[m_buffers.m_indices[tri_index * 3 + 1]];
    const glm::vec2 &st2 = m_buffers.m_texcoords[m_buffers.m_indices[tri_index * 3 + 2]];
    hit_texcoord = (1 - uv.x - uv.y) * st0 + uv.x * st1 + uv.y * st2;
}

bool Mesh::rayTriangleIntersect(const Ray & ray, const glm::vec3 & v0, const glm::vec3 & v1, const glm::vec3 & v2, float & t, float & u, float & v) const
{
    static const float EPSILON = 1e-8;

    glm::vec3 edge1 = v1 - v0;
    glm::vec3 edge2 = v2 - v0;
    glm::vec3 p_vec = glm::cross(ray.m_direction, edge2);

    float det = glm::dot(edge1, p_vec);

    if(det < EPSILON && det > -EPSILON)
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

    return (t >= 0.0f) ? true : false;
}
