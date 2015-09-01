#pragma once

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>
#include <string>

#include "Mesh.h"

class Model
{
public:
    Model();
    virtual ~Model();

    void loadModel(std::string & filename);
    void render();

    /* Primitives */
    void genCone    (float height      = 3.0f, float r             = 1.5f, unsigned int slices = 10, unsigned int stacks = 10);
    void genCube    (float radius      = 1.0f);
    void genCylinder(float height      = 3.0f, float radius        = 1.5f, unsigned int slices = 10);
    void genPlane   (float width       = 1.0f, float height        = 1.0f, unsigned int slices = 5, unsigned int stacks = 5);
    void genSphere  (float radius      = 1.5f, unsigned int slices = 12);
    void genTorus   (float innerRadius = 1.0f, float outerRadius   = 2.0f, unsigned int slices = 10, unsigned int stacks = 10);
    void genQuad    (float width       = 1.0f, float height        = 1.0f);
    
    void setTexture(std::string filename);

private:
    std::vector<Mesh *> meshes;
    enum ModelType { VE_PRIMITIVE, VE_MODEL };
    ModelType model_type;

    void processNode(aiNode * node, const aiScene * scene);
    Mesh * processMesh(aiMesh * mesh, const aiScene * scene);
    std::vector<Texture *> loadMaterialTextures(aiMaterial * mat, aiTextureType type, std::string type_name);
};

