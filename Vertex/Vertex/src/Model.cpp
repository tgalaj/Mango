#include "Model.h"
#include "GeomPrimitive.h"
#include "ShaderManager.h"
#include "CoreAssetManager.h"

#include <glm\gtc\matrix_transform.hpp>

Model::Model() 
{
    meshes.reserve(4);
    setMaterial("ve_basic");
}

Model::~Model()
{
}

void Model::loadModel(std::string filename)
{
    model_type = VE_MODEL;

    Assimp::Importer importer;

    unsigned int flags = aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals;
    const aiScene * scene = importer.ReadFile(filename, flags);

    if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        fprintf(stderr, "Assimp error while loading mesh %s\n Error: %s\n", filename.c_str(), importer.GetErrorString());
        return;
    }

    aiString directory = aiString(filename.substr(0, filename.rfind("/")));

    processNode(scene->mRootNode, scene, directory);
}

void Model::processNode(aiNode * node, const aiScene * scene, aiString & directory)
{
    for (GLuint i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh * mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene, directory));
    }

    for (GLuint i = 0; i < node->mNumChildren; ++i)
    {
        processNode(node->mChildren[i], scene, directory);
    }
}

Mesh * Model::processMesh(aiMesh * mesh, const aiScene * scene, aiString & directory)
{
    VEBuffers buffers;
    std::vector<Texture *> textures;

    Mesh * veMesh = new Mesh();

    /* Process positions, normals, texcoords */
    for (GLuint i = 0; i < mesh->mNumVertices; ++i)
    {
        buffers.positions.push_back(glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z));
        buffers.normals.push_back  (glm::vec3(mesh->mNormals[i].x,  mesh->mNormals[i].y,  mesh->mNormals[i].z));
        
        if (mesh->mTextureCoords[0])
        {
            buffers.texcoords.push_back(glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y));
        }
        else
        {
            buffers.texcoords.push_back(glm::vec2(0.0f));
        }
    }

    /* Process indices */
    for (GLuint i = 0; i < mesh->mNumFaces; ++i)
    {
        aiFace face = mesh->mFaces[i];
        veMesh->indices_count += face.mNumIndices;

        for (GLuint j = 0; j < face.mNumIndices; ++j)
        {
            buffers.indices.push_back(face.mIndices[j]);
        }
    }

    /* Process textures */
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial * material = scene->mMaterials[mesh->mMaterialIndex];

        std::vector<Texture *> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", directory);
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        std::vector<Texture *> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular", directory);
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }

    /* Feed Vertex Engine's Mesh with a data */
    veMesh->setBuffers(buffers);
    veMesh->textures = textures;

    return veMesh;
}

std::vector<Texture*> Model::loadMaterialTextures(aiMaterial * mat, aiTextureType type, std::string type_name, aiString & directory)
{
    std::vector<Texture*> textures;

    GLuint texturesCount = mat->GetTextureCount(type);

    if (texturesCount > 0)
    {
        for (GLuint i = 0; i < texturesCount; ++i)
        {
            aiString str, fullPath(directory);
            mat->GetTexture(type, i, &str);

            fullPath.Append("/");
            fullPath.Append(str.C_Str());

            Texture * texture = CoreAssetManager::createTexture2D(fullPath.C_Str());//new Texture();
            texture->setTypeName(type_name);
            //texture->createTexture2D(fullPath.C_Str());
            textures.push_back(texture);
        }
    }
    else
    {
        Texture * texture = nullptr;;
        
        if (type_name == "texture_diffuse")
        {
            texture = CoreAssetManager::createTexture2D("res/texture/diff_default.jpg");
        }
        else 
        if (type_name == "texture_specular")
        {
            texture = CoreAssetManager::createTexture2D("res/texture/spec_default.jpg");
        }
        else
        if (type_name == "texture_normal")
        {
            texture = CoreAssetManager::createTexture2D("res/texture/normal_default.jpg");
        }

        texture->setTypeName(type_name);
        textures.push_back(texture);
    }

    return textures;
}

void Model::render()
{
    shader->setUniformMatrix4fv("world", worldTransform);

    for (auto & mesh : meshes)
    {
        mesh->render(shader);
    }
}

void Model::genCone(float height, float radius, unsigned int slices, unsigned int stacks)
{
    model_type = VE_PRIMITIVE;

    VEBuffers buffers;
    GeomPrimitive::genCone(buffers, height, radius, slices, stacks);

    Mesh * mesh = new Mesh();
    mesh->setBuffers(buffers);
    mesh->indices_count = buffers.indices.size();

    meshes.clear();
    meshes.push_back(mesh);
    setTexture("res/texture/diff_default.jpg");
    setTexture("res/texture/spec_default.jpg", SPECULAR);
    //setTexture("res/texture/normal_default.jpg", NORMAL);
}

void Model::genCube(float radius)
{
    model_type = VE_PRIMITIVE;

    VEBuffers buffers;
    GeomPrimitive::genCube(buffers, radius);
    
    Mesh * mesh = new Mesh();
    mesh->setBuffers(buffers);
    mesh->indices_count = buffers.indices.size();

    meshes.clear();
    meshes.push_back(mesh);
    setTexture("res/texture/diff_default.jpg");
    setTexture("res/texture/spec_default.jpg", SPECULAR);
    //setTexture("res/texture/normal_default.jpg", NORMAL);
}

void Model::genCylinder(float height, float r, unsigned int slices)
{
    model_type = VE_PRIMITIVE;

    VEBuffers buffers;
    GeomPrimitive::genCylinder(buffers, height, r, slices);
    
    Mesh * mesh = new Mesh();
    mesh->setBuffers(buffers);
    mesh->indices_count = buffers.indices.size();

    meshes.clear();
    meshes.push_back(mesh);
    setTexture("res/texture/diff_default.jpg");
    setTexture("res/texture/spec_default.jpg", SPECULAR);
    //setTexture("res/texture/normal_default.jpg", NORMAL);
}

void Model::genPlane(float width, float height, unsigned int slices, unsigned int stacks)
{
    model_type = VE_PRIMITIVE;

    VEBuffers buffers;
    GeomPrimitive::genPlane(buffers, width, height, slices, stacks);
    
    Mesh * mesh = new Mesh();
    mesh->setBuffers(buffers);
    mesh->indices_count = buffers.indices.size();

    meshes.clear();
    meshes.push_back(mesh);
    setTexture("res/texture/diff_default.jpg");
    setTexture("res/texture/spec_default.jpg", SPECULAR);
    //setTexture("res/texture/normal_default.jpg", NORMAL);
}

void Model::genSphere(float radius, unsigned int slices)
{
    model_type = VE_PRIMITIVE;

    VEBuffers buffers;
    GeomPrimitive::genSphere(buffers, radius, slices);
    
    Mesh * mesh = new Mesh();
    mesh->setBuffers(buffers);
    mesh->indices_count = buffers.indices.size();

    meshes.clear();
    meshes.push_back(mesh);
    setTexture("res/texture/diff_default.jpg");
    setTexture("res/texture/spec_default.jpg", SPECULAR);
    //setTexture("res/texture/normal_default.jpg", NORMAL);
}

void Model::genTorus(float innerRadius, float outerRadius, unsigned int slices, unsigned int stacks)
{
    model_type = VE_PRIMITIVE;

    VEBuffers buffers;
    GeomPrimitive::genTorus(buffers, innerRadius, outerRadius, slices, stacks);
    
    Mesh * mesh = new Mesh();
    mesh->setBuffers(buffers);
    mesh->draw_mode     = GL_TRIANGLE_STRIP;
    mesh->indices_count = buffers.indices.size();

    meshes.clear();
    meshes.push_back(mesh);
    setTexture("res/texture/diff_default.jpg");
    setTexture("res/texture/spec_default.jpg", SPECULAR);
    //setTexture("res/texture/normal_default.jpg", NORMAL);
}

void Model::genQuad(float width, float height)
{
    model_type = VE_PRIMITIVE;

    VEBuffers buffers;
    GeomPrimitive::genQuad(buffers, width, height);

    Mesh * mesh = new Mesh();
    mesh->setBuffers(buffers);
    mesh->draw_mode     = GL_TRIANGLE_STRIP;
    mesh->indices_count = buffers.indices.size();

    meshes.clear();
    meshes.push_back(mesh);
    setTexture("res/texture/diff_default.jpg");
    setTexture("res/texture/spec_default.jpg", SPECULAR);
    //setTexture("res/texture/normal_default.jpg", NORMAL);
}

void Model::setMaterial(const std::string & shaderName)
{
    shader = ShaderManager::getShader(shaderName);
}

void Model::setTexture(const std::string & filename, TextureType texType)
{
    if (meshes.size() > 0 && model_type == VE_PRIMITIVE)
    {
        Texture * t = CoreAssetManager::createTexture2D(filename);//new Texture();
        //t->createTexture2D(filename);
        
        int idx = 0;

        switch (texType)
        {
            case DIFFUSE:
                t->setTypeName(std::string("texture_diffuse"));
                idx = 0;
                break;
            case SPECULAR:
                t->setTypeName(std::string("texture_specular"));
                idx = 1;
                break;
            case NORMAL:
                t->setTypeName(std::string("texture_normal"));
                idx = 2;
                break;
            case EMISSION:
                t->setTypeName(std::string("texture_emission"));
                idx = 3;
                break;
        }

        if (meshes[0]->textures.size() > idx)
        {
            meshes[0]->textures[idx] = t;
        }
        else
        {
            meshes[0]->textures.push_back(t);
        }
    }
}
