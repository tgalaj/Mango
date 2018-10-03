#pragma once

#include <glad/glad.h>
#include <glm/vec3.hpp>
#include <vector>

#include "Material.h"

namespace Vertex
{
    struct VertexBuffers
    {
        struct Vertex
        {
            glm::vec3 m_position;
            glm::vec3 m_normal;
            glm::vec3 m_texcoord;
            glm::vec3 m_tangent;
        };

        std::vector<Vertex>    m_vertices;
        std::vector<GLushort>  m_indices;
    };

    struct MeshData
    {
        MeshData();
        ~MeshData();

        GLuint m_vao_id;
        GLuint m_vbo_ids[2];
        GLuint m_indices_count;
        GLenum m_draw_mode;
    };

    class Mesh
    {
    public:
        Mesh();
        ~Mesh();

        void setBuffers(const VertexBuffers & buffers);
        void setDrawMode(GLenum draw_mode) const { m_mesh_data->m_draw_mode = draw_mode; }

        GLenum getDrawMode()     const { return m_mesh_data->m_draw_mode; }
        GLuint getIndicesCount() const { return m_mesh_data->m_indices_count; }

        void render() const;

        Material m_material;

    private:
        std::shared_ptr<MeshData> m_mesh_data;

        enum { VERTEX_DATA, INDEX };
    };
}
