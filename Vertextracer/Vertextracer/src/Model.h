#pragma once

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>

#include "Mesh.h"

class Model
{
public:
    ~Model();

    void loadModel(const std::string & file_name);

    std::vector<Mesh *> m_meshes;

private:
    void processNode(aiNode * node, const aiScene * scene, aiString & directory);
    Mesh * processMesh(aiMesh * mesh, const aiScene * scene, aiString & directory);
};
