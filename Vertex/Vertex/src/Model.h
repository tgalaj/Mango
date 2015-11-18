#pragma once

#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>
#include <string>

#include "Mesh.h"
#include "SceneNode.h"

class Model : public SceneNode
{
public:
    friend class Renderer;
    friend class CoreAssetManager;

    enum TextureType { DIFFUSE, SPECULAR, NORMAL, EMISSION };

    virtual ~Model();

    void setShader      (const std::string & shaderName);
    void setShininess   (int meshIndex, float shininess);
    void setDiffuseColor(int meshIndex, glm::vec3 diffuseColor);

    /* Primitives */
    void genCone    (float height      = 3.0f, float r             = 1.5f, unsigned int slices = 10, unsigned int stacks = 10);
    void genCube    (float radius      = 1.0f);
    void genCylinder(float height      = 3.0f, float radius        = 1.5f, unsigned int slices = 10);
    void genPlane   (float width       = 1.0f, float height        = 1.0f, unsigned int slices = 5, unsigned int stacks = 5);
    void genSphere  (float radius      = 1.5f, unsigned int slices = 12);
    void genTorus   (float innerRadius = 1.0f, float outerRadius   = 2.0f, unsigned int slices = 10, unsigned int stacks = 10);
    void genQuad    (float width       = 1.0f, float height        = 1.0f);
    
    void setTexture(const std::string & filename, TextureType texType = DIFFUSE);

private:
    Model();

    enum ModelType { VE_PRIMITIVE, VE_MODEL };

    std::vector<Mesh *> meshes;
    Shader * shader;
    ModelType model_type;

    void processNode(aiNode * node, const aiScene * scene, aiString & directory);
    Mesh * processMesh(aiMesh * mesh, const aiScene * scene, aiString & directory);
    std::vector<Texture *> loadMaterialTextures(aiMaterial * mat, aiTextureType type, std::string type_name, aiString & directory);

    void render();
    void loadModel(std::string filename);
};

