#pragma once
#include "Object.h"

class Mesh : public Object
{
public:
    struct Buffers
    {
        std::vector<glm::vec3> m_positions;
        std::vector<glm::vec3> m_normals;
        std::vector<glm::vec2> m_texcoords;
        std::vector<uint16_t>  m_indices;
        size_t m_num_faces;
    };

    explicit Mesh(const Buffers & buffers);

    bool intersect(const Ray & ray, float & t_near, uint32_t & tri_index, glm::vec2 & texcoord) const override;
    void getSurfaceData(const glm::vec3 & hit_point, const glm::vec3 & view_dir, const uint32_t & tri_index, const glm::vec2 & uv, glm::vec3 & hit_normal, glm::vec2 & hit_texcoord) const override;

private:
    /* Moller-Trumbore ray triangle intersection algorithm */
    bool rayTriangleIntersect(const Ray & ray, const glm::vec3 & v0, const glm::vec3 & v1, const glm::vec3 & v2, float & t, float & u, float & v) const;

    Buffers m_buffers;
};
