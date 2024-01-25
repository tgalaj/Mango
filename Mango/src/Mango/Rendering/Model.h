#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <memory>

#include "Material.h"
#include "Mesh.h"
#include "Shader.h"

namespace mango
{
    struct PrimitiveProperties
    {
        float    width       = 1.0f;
        float    height      = 1.0f;
        float    radius      = 1.0f;
        float    size        = 1.0f;
        float    innerRadius = 1.0f;
        float    outerRadius = 2.0f;
        int32_t  slices      = 10;
        int32_t  stacks      = 10;
    }; 

    class Model
    {
    public:
        enum class ModelType { Model3D, Cone, Cube, Cylinder, Plane, Sphere, Torus, Quad, None };

    public:
        Model() = default;
        Model(const std::string& filename);
        virtual ~Model() = default;

        /* Primitives */
        void genCone    (float height = 3.0f, float radius = 1.5f, unsigned int slices = 10, unsigned int stacks = 10);
        void genCube    (float size = 1.0f);
        void genCylinder(float height = 3.0f, float radius = 1.5f, unsigned int slices = 10);
        void genPlane   (float width  = 1.0f, float height = 1.0f, unsigned int slices = 5, unsigned int stacks = 5);
        void genSphere  (float radius = 1.5f, unsigned int slices = 12);
        void genTorus   (float innerRadius = 1.0f, float outerRadius = 2.0f, unsigned int slices = 10, unsigned int stacks = 10);
        void genQuad    (float width = 1.0f, float height = 1.0f);

        void load(const std::string & filename);
        void render(Shader & shader);

        ModelType getModelType() const { return m_modelType; }
        void setModelType(ModelType type);

        void setDrawMode(GLenum drawMode);
        GLenum getDrawMode() { return getMesh(0).getDrawMode(); }

        PrimitiveProperties getPrimitiveProperties() const { return m_primitiveProperties; }
        void setPrimitiveProperties(PrimitiveProperties properties) { m_primitiveProperties = properties; regeneratePrimitive(); }

        std::string getFilename() const { return m_filename; }

        Mesh & getMesh(unsigned int index = 0)
        {
            if (index > m_meshes.size())
            {
                index = m_meshes.size() - 1;
            }

            return m_meshes[index];
        }

        unsigned meshesCount() const { return m_meshes.size(); }
    
    public:
        std::string m_filename = "";

    private:
        void calcTangentSpace(VertexBuffers & buffers) const;
        void genPrimitive(VertexBuffers & buffers, bool calcTangents = true);
        void regeneratePrimitive();

        void processNode(aiNode * node, const aiScene * scene, aiString & directory);
        Mesh processMesh(aiMesh * mesh, const aiScene * scene, aiString & directory) const;

        void loadMaterialTextures(Mesh & mesh, aiMaterial * mat, aiTextureType type, Material::TextureType textureType, aiString & directory) const;

    private:
        std::vector<Mesh>   m_meshes;
        ModelType           m_modelType = ModelType::None;
        PrimitiveProperties m_primitiveProperties{};
    };
}
