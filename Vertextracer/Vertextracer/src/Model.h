#pragma once

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#include "Mesh.h"

class Model
{
public:
    explicit Model(const glm::mat4 & transform = glm::mat4(1.0f)) 
        : m_model_matrix (transform), 
          m_normal_matrix(glm::transpose(glm::inverse(transform))) {}

    ~Model();

    void loadModel(const std::string & file_name);

    std::vector<Mesh *> m_meshes;

private:
    void processNode(aiNode * node, const aiScene * scene, aiString & directory);
    Mesh * processMesh(aiMesh * mesh, const aiScene * scene, aiString & directory);

    glm::mat4 m_model_matrix;
    glm::mat3 m_normal_matrix;
};
