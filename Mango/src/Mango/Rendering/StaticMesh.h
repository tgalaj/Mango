#pragma once
#include "Material.h"
#include "StaticMesh.h"
#include "Shader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace mango
{
    struct PrimitiveProperties
    {
        float    width         = 1.0f;
        float    height        = 1.0f;
        float    radius        = 1.0f;
        float    size          = 1.0f;
        float    innerRadius   = 1.0f;
        float    outerRadius   = 2.0f;
        int32_t  slices        = 10;
        int32_t  stacks        = 10;
        int32_t  p             = 2;
        int32_t  q             = 3;
    }; 

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

        ref<Material> material      = nullptr;
        uint32_t      baseVertex    = 0;
        uint32_t      baseIndex     = 0;
        uint32_t      indicesCount  = 0;
    };

    class StaticMesh
    {
    public:
        enum class MeshType { StaticMesh, AnimatedMesh, Cone, Cube, Cylinder, Plane, PQTorusKnot, Sphere, Torus, TrefoilKnot, Quad };

        enum class DrawMode { POINTS         = GL_POINTS, 
                              LINES          = GL_LINES, 
                              TRIANGLES      = GL_TRIANGLES, 
                              TRIANGLE_STRIP = GL_TRIANGLE_STRIP,
                              PATCHES        = GL_PATCHES };

    public:
        StaticMesh()
            : m_unitScale(1),
              m_vaoName  (0),
              m_vboName  (0),
              m_iboName  (0),
              m_drawMode (DrawMode::TRIANGLES)
        {
        }

        virtual ~StaticMesh() { release(); }

        StaticMesh           (const StaticMesh&) = delete;
        StaticMesh& operator=(const StaticMesh&) = delete;

        StaticMesh(StaticMesh&& other) noexcept
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

        StaticMesh& operator=(StaticMesh&& other) noexcept
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

        virtual void render(uint32_t instancesCount = 0);
        virtual void render(ref<Shader>& shader, uint32_t instancesCount = 0);

        void addAttributeBuffer(GLuint attribIndex, GLuint bindingIndex, GLint formatSize, GLenum dataType, GLuint bufferID, GLsizei stride, GLuint divisor = 0);
        void setMaterial(ref<Material>& material, uint32_t meshID = 0);
        
        void     setDrawMode(DrawMode mode) { m_drawMode = mode; }
        DrawMode getDrawMode()              { return m_drawMode; }

        MeshType getMeshType() const        { return m_modelType; }
        void     setMeshType(MeshType type);

        float getUnitScaleFactor() const { return m_unitScale; }
        std::string getFilename()  const { return m_filename; }

        Submesh& getSubmesh(unsigned int index = 0)
        {
            if (index > m_submeshes.size())
            {
                index = m_submeshes.size() - 1;
            }

            return m_submeshes[index];
        }

        unsigned getSubmeshCount() const { return m_submeshes.size(); }

        /* Primitives */
        void genCone       (float    height      = 3.0f, float radius         = 1.5f, uint32_t slices = 10, uint32_t stacks = 10);
        void genCube       (float    size        = 1.0f, float texcoordScale = 1.0f);
        void genCubeMap    (float    radius      = 1.0f);                     
        void genCylinder   (float    height      = 3.0f, float    radius      = 1.5f, uint32_t slices = 10);
        void genPlane      (float    width       = 1.0f, float    height      = 1.0f, uint32_t slices = 5, uint32_t stacks = 5);
        void genPlaneGrid  (float    width       = 1.0f, float    height      = 1.0f, uint32_t slices = 5, uint32_t stacks = 5);
        void genSphere     (float    radius      = 1.5f, uint32_t slices      = 12);
        void genTorus      (float    innerRadius = 1.0f, float    outerRadius = 2.0f, uint32_t slices = 10, uint32_t stacks = 10);
        void genTrefoilKnot(uint32_t slices      = 100,  uint32_t stacks      = 20);
        void genPQTorusKnot(uint32_t slices      = 256,  uint32_t stacks      = 16,   int p = 2, int q = 3, float knotR = 0.75, float tubeR = 0.15);
        void genQuad       (float    width       = 1.0f, float    height      = 1.0f);

        PrimitiveProperties getPrimitiveProperties() const { return m_primitiveProperties; }

        void setPrimitiveProperties(PrimitiveProperties properties)
        {
            m_primitiveProperties = properties;
            regeneratePrimitive();
        }

        void setPrimitiveProperties(MeshType modelType, PrimitiveProperties properties) 
        { 
            m_modelType           = modelType;
            m_primitiveProperties = properties; 
            regeneratePrimitive(); 
        }
    
    public:
        std::string m_filename = "";

    protected:
        struct VertexData
        {
            std::vector<glm::vec3> positions;
            std::vector<glm::vec2> texcoords;
            std::vector<glm::vec3> normals;
            std::vector<glm::vec3> tangents;
            std::vector<uint32_t>  indices;
        };

        // For converting between ASSIMP and glm
        static inline glm::vec3 vec3_cast(const aiVector3D&   v) { return glm::vec3(v.x, v.y, v.z); }
        static inline glm::vec2 vec2_cast(const aiVector3D&   v) { return glm::vec2(v.x, v.y); }
        static inline glm::quat quat_cast(const aiQuaternion& q) { return glm::quat(q.w, q.x, q.y, q.z); }
        static inline glm::mat4 mat4_cast(const aiMatrix4x4&  m) { return glm::transpose(glm::make_mat4(&m.a1)); }
        static inline glm::mat4 mat4_cast(const aiMatrix3x3&  m) { return glm::transpose(glm::make_mat3(&m.a1)); }

        void regeneratePrimitive();

        virtual bool load(const std::string& filename);
        virtual bool parseScene(const aiScene* scene, const std::filesystem::path& parentDirectory);
        virtual void loadMeshPart(const aiMesh* mesh, VertexData& vertexData);
        virtual bool loadMaterials(const aiScene* scene, const std::filesystem::path& parentDirectory);
        virtual bool loadMaterialTextures(const aiScene* scene, const aiMaterial* material, ref<Material>& mangoMaterial, aiTextureType type, Material::TextureType texture_type, const std::filesystem::path& parentDirectory) const;
        virtual void createBuffers(VertexData& vertexData);

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

        float    m_unitScale = 1.0f;
        GLuint   m_vaoName   = 0;
        GLuint   m_vboName   = 0;
        GLuint   m_iboName   = 0;
        DrawMode m_drawMode  = DrawMode::TRIANGLES;

        MeshType            m_modelType = MeshType::Cube;
        PrimitiveProperties m_primitiveProperties{};

    private:
        friend class AssetManager;
    };
}
