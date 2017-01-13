#pragma once

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#include "Mesh.h"

enum class MaterialType { DIFFUSE, REFLECTION, REFLECTION_AND_REFRACTION };

class Model
{
public:
    explicit Model(const glm::mat4   & transform = glm::mat4(1.0f),
                   const std::string & file_name = "",
                   const glm::vec3   & albedo    = glm::vec3(0.18f))
        : m_albedo(albedo), 
          m_type(MaterialType::DIFFUSE),
          m_model_matrix (transform),
          m_normal_matrix(glm::transpose(glm::inverse(transform)))

    {
        if(!file_name.empty())
        {
            loadModel(file_name);
        }
    }

    ~Model();

    void loadModel(const std::string & file_name);

    std::vector<Mesh *> m_meshes;
    glm::vec3 m_albedo;
    MaterialType m_type;

private:
    void processNode(aiNode * node, const aiScene * scene, aiString & directory);
    Mesh * processMesh(aiMesh * mesh, const aiScene * scene, aiString & directory);

    glm::mat4 m_model_matrix;
    glm::mat3 m_normal_matrix;
};
