#include "Model.h"

Model::~Model()
{
    for(size_t i = 0; i < m_meshes.size(); ++i)
    {
        delete m_meshes[i];
    }

    m_meshes.clear();
}

void Model::loadModel(const std::string & file_name)
{
    Assimp::Importer importer;

    unsigned int flags = aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs;
    const aiScene * scene = importer.ReadFile(file_name, flags);

    if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        fprintf(stderr, "Assimp error while loading mesh %s\n Error: %s\n", file_name.c_str(), importer.GetErrorString());
        return;
    }

    aiString directory = aiString(file_name.substr(0, file_name.rfind("/")));

    processNode(scene->mRootNode, scene, directory);
}

void Model::processNode(aiNode * node, const aiScene * scene, aiString & directory)
{
    for (size_t i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh * mesh = scene->mMeshes[node->mMeshes[i]];
        m_meshes.push_back(processMesh(mesh, scene, directory));
    }

    for (size_t i = 0; i < node->mNumChildren; ++i)
    {
        processNode(node->mChildren[i], scene, directory);
    }
}

Mesh * Model::processMesh(aiMesh * mesh, const aiScene * scene, aiString & directory)
{
    Mesh::Buffers buffers;

    /* Process positions, normals, texcoords */
    for (size_t i = 0; i < mesh->mNumVertices; ++i)
    {
        glm::vec4 position = m_model_matrix  * glm::vec4(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z, 1.0f);
        glm::vec3 normal   = m_normal_matrix * glm::vec3(mesh->mNormals[i].x,  mesh->mNormals[i].y,  mesh->mNormals[i].z);

        buffers.m_positions.push_back(glm::vec3(position));
        buffers.m_normals.push_back(normal);

        if (mesh->mTextureCoords[0])
        {
            buffers.m_texcoords.push_back(glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y));
        }
        else
        {
            buffers.m_texcoords.push_back(glm::vec2(0.0f));
        }
    }

    /* Process indices */
    for (size_t i = 0; i < mesh->mNumFaces; ++i)
    {
        aiFace face = mesh->mFaces[i];

        for (size_t j = 0; j < face.mNumIndices; ++j)
        {
            buffers.m_indices.push_back(face.mIndices[j]);
        }
    }

    buffers.m_num_faces = mesh->mNumFaces;

    /* Feed Vertex Engine's Mesh with a data */
    Mesh * veMesh = new Mesh(buffers);

    return veMesh;
}
