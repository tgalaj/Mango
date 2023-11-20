#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <memory>

#include "Material.h"
#include "Mesh.h"
#include "Shader.h"

namespace mango
{
    class Model
    {
    public:
        Model();
        virtual ~Model();

        /* Primitives */
        void genCone    (float height = 3.0f, float r = 1.5f, unsigned int slices = 10, unsigned int stacks = 10);
        void genCube    (float radius = 1.0f);
        void genCylinder(float height = 3.0f, float radius = 1.5f, unsigned int slices = 10);
        void genPlane   (float width  = 1.0f, float height = 1.0f, unsigned int slices = 5, unsigned int stacks = 5);
        void genSphere  (float radius = 1.5f, unsigned int slices = 12);
        void genTorus   (float innerRadius = 1.0f, float outerRadius = 2.0f, unsigned int slices = 10, unsigned int stacks = 10);
        void genQuad    (float width = 1.0f, float height = 1.0f);

        void load(const std::filesystem::path & filepath);
        void render(Shader & shader);

        void setDrawMode(GLenum drawMode);
        GLenum getDrawMode() { return getMesh(0).getDrawMode(); }

        Mesh & getMesh(unsigned int index = 0)
        {
            if (index > m_meshes.size())
            {
                index = m_meshes.size() - 1;
            }

            return m_meshes[index];
        }

        unsigned meshesCount() const { return m_meshes.size(); }

    private:
        void calcTangentSpace(VertexBuffers & buffers) const;
        void genPrimitive(VertexBuffers & buffers, bool calcTangents = true);

        void processNode(aiNode * node, const aiScene * scene, aiString & directory);
        Mesh processMesh(aiMesh * mesh, const aiScene * scene, aiString & directory) const;

        void loadMaterialTextures(Mesh & mesh, aiMaterial * mat, aiTextureType type, Material::TextureType textureType, aiString & directory) const;

        std::vector<Mesh> m_meshes;
    };
}
