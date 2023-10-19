#include "mgpch.h"

#include <assimp/postprocess.h>
#include <glm/gtc/matrix_transform.hpp>

#include "Model.h"
#include "CoreEngine/CoreAssetManager.h"
#include "CoreEngine/CoreServices.h"
#include "Utilities/GeomPrimitive.h"

namespace mango
{
    Model::Model()
    {
    }

    Model::~Model()
    {
    }

    void Model::load(const std::filesystem::path & filepath)
    {
        Assimp::Importer importer;

        unsigned int flags = aiProcess_Triangulate              | 
                             aiProcess_GenSmoothNormals         | 
                             aiProcess_CalcTangentSpace         | 
                             aiProcess_FlipUVs                  | 
                             aiProcess_PreTransformVertices     | 
                             aiProcess_RemoveRedundantMaterials | 
                             aiProcess_ImproveCacheLocality     | 
                             aiProcess_JoinIdenticalVertices;
        const aiScene * scene = importer.ReadFile(filepath.string(), flags);

        if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            fprintf(stderr, "Assimp error while loading mesh %s\n Error: %s\n", filepath.string().c_str(), importer.GetErrorString());
            return;
        }

        aiString directory = aiString(filepath.parent_path().string());

        processNode(scene->mRootNode, scene, directory);
    }

    void Model::processNode(aiNode * node, const aiScene * scene, aiString & directory)
    {
        for (GLuint i = 0; i < node->mNumMeshes; ++i)
        {
            aiMesh * mesh = scene->mMeshes[node->mMeshes[i]];
            m_meshes.push_back(processMesh(mesh, scene, directory));
        }

        for (GLuint i = 0; i < node->mNumChildren; ++i)
        {
            processNode(node->mChildren[i], scene, directory);
        }
    }

    Mesh Model::processMesh(aiMesh * mesh, const aiScene * scene, aiString & directory) const
    {
        VertexBuffers buffers;
        Mesh veMesh;

        /* Process positions, normals, texcoords */
        for (GLuint i = 0; i < mesh->mNumVertices; ++i)
        {
            VertexBuffers::Vertex vertex;
            vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
            vertex.normal   = glm::vec3(mesh->mNormals[i].x,  mesh->mNormals[i].y,  mesh->mNormals[i].z);
            vertex.tangent  = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);

            if (mesh->mTextureCoords[0])
            {
                vertex.texcoord = glm::vec3(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y, 0.0f);
            }
            else
            {
                vertex.texcoord = glm::vec3(0.0f);
            }

            buffers.vertices.push_back(vertex);
        }

        /* Process indices */
        for (GLuint i = 0; i < mesh->mNumFaces; ++i)
        {
            aiFace face = mesh->mFaces[i];

            for (GLuint j = 0; j < face.mNumIndices; ++j)
            {
                buffers.indices.push_back(face.mIndices[j]);
            }
        }

        /* Process textures */
        if (mesh->mMaterialIndex >= 0)
        {
            aiMaterial * material = scene->mMaterials[mesh->mMaterialIndex];

            loadMaterialTextures(veMesh, material, aiTextureType_DIFFUSE,  Material::TextureType::DIFFUSE,  directory);
            loadMaterialTextures(veMesh, material, aiTextureType_HEIGHT,   Material::TextureType::NORMAL,   directory);
            loadMaterialTextures(veMesh, material, aiTextureType_NORMALS,  Material::TextureType::NORMAL,   directory);
            loadMaterialTextures(veMesh, material, aiTextureType_SPECULAR, Material::TextureType::SPECULAR, directory);
        }

        /* Feed engine's Mesh with data */
        veMesh.setBuffers(buffers);

        return veMesh;
    }

    void Model::loadMaterialTextures(Mesh & mesh, aiMaterial * mat, aiTextureType type, Material::TextureType textureType, aiString & directory) const
    {
        GLuint texturesCount = mat->GetTextureCount(type);

        if (texturesCount > 0)
        {
            for (GLuint i = 0; i < texturesCount; ++i)
            {
                aiString str, fullPath(directory);
                mat->GetTexture(type, i, &str);

                fullPath.Append("/");
                fullPath.Append(str.C_Str());

                auto texture = CoreAssetManager::createTexture2D(fullPath.C_Str(), type == aiTextureType_DIFFUSE);
                mesh.material.addTexture(textureType, texture);
            }
        }
    }

    void Model::calcTangentSpace(VertexBuffers & buffers) const
    {
        for (unsigned i = 0; i < buffers.vertices.size(); ++i)
        {
            buffers.vertices[i].tangent = glm::vec3(0.0f);
        }

        for (unsigned i = 0; i < buffers.indices.size(); i += 3)
        {
            auto i0 = buffers.indices[i];
            auto i1 = buffers.indices[i + 1];
            auto i2 = buffers.indices[i + 2];

            auto edge1 = buffers.vertices[i1].position - buffers.vertices[i0].position;
            auto edge2 = buffers.vertices[i2].position - buffers.vertices[i0].position;

            auto deltaUV1 = buffers.vertices[i1].texcoord - buffers.vertices[i0].texcoord;
            auto deltaUV2 = buffers.vertices[i2].texcoord - buffers.vertices[i0].texcoord;

            float dividend = (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
                  dividend = (dividend == 0.0f) ? 1e-5f : dividend;

            float f = 1.0f / dividend;

            glm::vec3 tangent(0.0f);
            tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
            tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
            tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

            buffers.vertices[i0].tangent += tangent;
            buffers.vertices[i1].tangent += tangent;
            buffers.vertices[i2].tangent += tangent;
        }

        for (unsigned i = 0; i < buffers.vertices.size(); ++i)
        {
            buffers.vertices[i].tangent = glm::normalize(buffers.vertices[i].tangent);
        }
    }

    void Model::genPrimitive(VertexBuffers & buffers, bool calcTangents /*= true*/)
    {
        Mesh mesh;

        if (calcTangents) calcTangentSpace(buffers);
        mesh.setBuffers(buffers);

        if(m_meshes.size() > 0)
        {
            return;
        }

        m_meshes.push_back(mesh);
    }

    void Model::genCone(float height, float radius, unsigned int slices, unsigned int stacks)
    {
        VertexBuffers buffers;
        GeomPrimitive::genCone(buffers, height, radius, slices, stacks);

        genPrimitive(buffers);
    }

    void Model::genCube(float radius)
    {
        VertexBuffers buffers;
        GeomPrimitive::genCube(buffers, radius);

        genPrimitive(buffers);
    }

    void Model::genCylinder(float height, float r, unsigned int slices)
    {
        VertexBuffers buffers;
        GeomPrimitive::genCylinder(buffers, height, r, slices);

        genPrimitive(buffers);
    }

    void Model::genPlane(float width, float height, unsigned int slices, unsigned int stacks)
    {
        VertexBuffers buffers;
        GeomPrimitive::genPlane(buffers, width, height, slices, stacks);

        genPrimitive(buffers);
    }

    void Model::genSphere(float radius, unsigned int slices)
    {
        VertexBuffers buffers;
        GeomPrimitive::genSphere(buffers, radius, slices);

        genPrimitive(buffers);
    }

    void Model::genTorus(float innerRadius, float outerRadius, unsigned int slices, unsigned int stacks)
    {
        VertexBuffers buffers;
        GeomPrimitive::genTorus(buffers, innerRadius, outerRadius, slices, stacks);

        genPrimitive(buffers);
    }

    void Model::genQuad(float width, float height)
    {
        VertexBuffers buffers;
        GeomPrimitive::genQuad(buffers, width, height);

        // quad has its own predefined tangents
        genPrimitive(buffers, false);
        m_meshes[m_meshes.size() - 1].setDrawMode(GL_TRIANGLE_STRIP);
    }

    void Model::render(Shader & shader)
    {
        for(unsigned i = 0; i < m_meshes.size(); ++i)
        {
            shader.updateUniforms(m_meshes[i].material);
            m_meshes[i].render();
        }
    }

    void Model::setDrawMode(GLenum drawMode)
    {
        for(unsigned i = 0; i < m_meshes.size(); ++i)
        {
            m_meshes[i].setDrawMode(drawMode);
        }
    }
}
