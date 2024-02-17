#pragma once

#include <glad/glad.h>
#include <glm/vec3.hpp>
#include <vector>

#include "Material.h"

namespace mango
{
    struct VertexBuffers
    {
        struct Vertex
        {
            glm::vec3 position{};
            glm::vec3 normal{};
            glm::vec3 texcoord{};
            glm::vec3 tangent{};
        };

        std::vector<Vertex> vertices{};
        std::vector<GLuint> indices{};
    };

    struct MeshData
    {
        MeshData();
        ~MeshData();

        GLuint vao;
        GLuint vbos[2];
        GLuint indicesCount;
        GLenum drawMode;
    };

    class Mesh
    {
    public:
        Mesh();
        ~Mesh();

        void setBuffers(const VertexBuffers & buffers);
        void setDrawMode(GLenum drawMode) const { m_meshData->drawMode = drawMode; }

        GLenum getDrawMode()     const { return m_meshData->drawMode; }
        GLuint getIndicesCount() const { return m_meshData->indicesCount; }

        void render() const;

        Material material;

    private:
        ref<MeshData> m_meshData;

        enum { VERTEX_DATA, INDEX };
    };
}
