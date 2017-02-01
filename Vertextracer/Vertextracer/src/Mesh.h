#pragma once
#include "Object.h"
#include <vector>

class Mesh : public Object
{
public:
    struct Buffers
    {
        std::vector<glm::highp_dvec3> m_positions;
        std::vector<glm::highp_dvec3> m_normals;
        std::vector<glm::highp_dvec2> m_texcoords;
        std::vector<uint16_t>  m_indices;
        size_t m_num_faces;
    };

    explicit Mesh(const Buffers & buffers);

    bool intersect(const Ray & ray, double & t_near, uint32_t & tri_index, glm::highp_dvec2 & texcoord) const override;
    void getSurfaceData(const glm::highp_dvec3 & hit_point, const glm::highp_dvec3 & view_dir, const uint32_t & tri_index, const glm::highp_dvec2 & uv, glm::highp_dvec3 & hit_normal, glm::highp_dvec2 & hit_texcoord, bool flat_shading) const override;

private:
    /* Moller-Trumbore ray triangle intersection algorithm */
    bool rayTriangleIntersect(const Ray & ray, const glm::highp_dvec3 & v0, const glm::highp_dvec3 & v1, const glm::highp_dvec3 & v2, double & t, double & u, double & v) const;

    Buffers m_buffers;
};
