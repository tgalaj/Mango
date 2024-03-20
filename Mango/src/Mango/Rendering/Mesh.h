#pragma once
#include "Material.h"
#include "Mesh.h"
#include "Shader.h"

#include <glm/glm.hpp>

namespace mango
{
    struct Vertex
    {
        Vertex() = default;

        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texcoord;
        glm::vec3 tangent;
    };

    struct Submesh
    {
    public:
        Submesh() = default;

        int32_t  materialIndex = -1;
        uint32_t baseVertex    = 0;
        uint32_t baseIndex     = 0;
        uint32_t indicesCount  = 0;
    };

    class Mesh
    {
    public:
        enum class DrawMode { POINTS         = GL_POINTS, 
                              LINES          = GL_LINES, 
                              TRIANGLES      = GL_TRIANGLES, 
                              TRIANGLE_STRIP = GL_TRIANGLE_STRIP,
                              PATCHES        = GL_PATCHES };

    public:
        Mesh(const std::string& name = "")
            : m_unitScale(1),
              m_vaoName  (0),
              m_vboName  (0),
              m_iboName  (0),
              m_drawMode (DrawMode::TRIANGLES),
              m_name     (name)
        {
        }

        virtual ~Mesh() { release(); }

        Mesh           (const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;

        Mesh(Mesh&& other) noexcept
            : m_submeshes(std::move(other.m_submeshes)),
              m_unitScale(other.m_unitScale),
              m_vaoName  (other.m_vaoName),
              m_vboName  (other.m_vboName),
              m_iboName  (other.m_iboName),
              m_drawMode (other.m_drawMode)
        {
            other.m_unitScale = 1;
            other.m_vaoName   = 0;
            other.m_vboName   = 0;
            other.m_iboName   = 0;
            other.m_drawMode  = DrawMode::TRIANGLES;
        }

        Mesh& operator=(Mesh&& other) noexcept
        {
            if (this != &other)
            {
                release();

                std::swap(m_submeshes, other.m_submeshes);
                std::swap(m_unitScale, other.m_unitScale);
                std::swap(m_vaoName,   other.m_vaoName);
                std::swap(m_vboName,   other.m_vboName);
                std::swap(m_iboName,   other.m_iboName);
                std::swap(m_drawMode,  other.m_drawMode);
            }

            return *this;
        }

        void bind() const;
        void render(uint32_t submeshIndex = 0, uint32_t instancesCount = 0);

        void addAttributeBuffer(GLuint attribIndex, GLuint bindingIndex, GLint formatSize, GLenum dataType, GLuint bufferID, GLsizei stride, GLuint divisor = 0);
        
        void     setDrawMode(DrawMode mode) { m_drawMode = mode; }
        DrawMode getDrawMode()              { return m_drawMode; }

        float getUnitScaleFactor() const { return m_unitScale; }
        std::string getName()      const { return m_name; }

        Submesh* getSubmesh(unsigned int index = 0)
        {
            if (m_submeshes.empty()) return nullptr;

            if (index > m_submeshes.size())
            {
                index = m_submeshes.size() - 1;
            }

            return &m_submeshes[index];
        }

        std::vector<Submesh>& getSubmeshes()          { return m_submeshes; }
        uint32_t              getSubmeshCount() const { return m_submeshes.size(); }

        MaterialTable getMaterials() { return m_materialTable; }

        /* Primitives */
        void genCone       (float    height      = 3.0f, float radius        = 1.5f, uint32_t slices = 10, uint32_t stacks = 10);
        void genCube       (float    size        = 1.0f, float texcoordScale = 1.0f);
        void genCubeMap    (float    radius      = 1.0f);                     
        void genCylinder   (float    height      = 3.0f, float    radius      = 1.5f, uint32_t slices = 10);
        void genPlane      (uint32_t resolution  = 10);
        // UV Sphere
        void genSphere     (float    radius      = 1.5f, uint32_t slices      = 12);
        // Octasphere based on: https://www.binpress.com/creating-octahedron-sphere-unity/
        void genSphere     (uint32_t divisions   = 3);
        void genTorus      (float    innerRadius = 1.0f, float    outerRadius = 2.0f, uint32_t slices = 10, uint32_t stacks = 10);
        void genPQTorusKnot(uint32_t slices      = 256,  uint32_t stacks      = 16,   int p = 2, int q = 3, float knotR = 0.75, float tubeR = 0.15);
        void genQuad       ();

    protected:
        struct VertexData
        {
            std::vector<glm::vec3> positions;
            std::vector<glm::vec2> texcoords;
            std::vector<glm::vec3> normals;
            std::vector<glm::vec3> tangents;
            std::vector<uint32_t>  indices;
        };
        void createBuffers(VertexData& vertexData);

        void calcTangentSpace(VertexData& vertexData);
        void genPrimitive(VertexData& vertexData, bool generateTangents = true);

        void release()
        {
            m_unitScale = 1.0;

            glDeleteBuffers(1, &m_vboName);
            m_vboName = 0;

            glDeleteBuffers(1, &m_iboName);
            m_iboName = 0;

            glDeleteVertexArrays(1, &m_vaoName);
            m_vaoName = 0;

            m_drawMode = DrawMode::TRIANGLES;

            m_submeshes.clear();
        }

    protected:
        std::vector<Submesh> m_submeshes;
        MaterialTable        m_materialTable;

        std::string m_name      = "";
        float       m_unitScale = 1.0f;
        GLuint      m_vaoName   = 0;
        GLuint      m_vboName   = 0;
        GLuint      m_iboName   = 0;
        DrawMode    m_drawMode  = DrawMode::TRIANGLES;

    private:
        friend class AssimpMeshImporter;
    };
}
