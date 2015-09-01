#include "Model.h"
#include "GeomPrimitive.h"

//todo set default texture
Model::Model() 
{
    meshes.reserve(4);
}

Model::~Model()
{
    for (auto & mesh : meshes)
    {
        delete mesh;
        mesh = nullptr;
    }
}

void Model::loadModel(std::string & filename)
{
    model_type = VE_MODEL;

    Assimp::Importer importer;

    unsigned int flags    = aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals;
    const aiScene * scene = importer.ReadFile(filename, flags);

    if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        fprintf(stderr, "Assimp error while loading mesh %s\n Error: %s\n", filename.c_str(), importer.GetErrorString());
        return;
    }

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode * node, const aiScene * scene)
{
    for (GLuint i = 0; i < node->mNumMeshes; ++i)
    {
        aiMesh * mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }

    for (GLuint i = 0; i < node->mNumChildren; ++i)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh * Model::processMesh(aiMesh * mesh, const aiScene * scene)
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

        std::vector<Texture *> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

        std::vector<Texture *> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }

    /* Feed Vertex Engine's Mesh with a data */
    veMesh->setBuffers(buffers);

    return veMesh;
}

std::vector<Texture*> Model::loadMaterialTextures(aiMaterial * mat, aiTextureType type, std::string type_name)
{
    std::vector<Texture*> textures;

    GLuint texturesCount = mat->GetTextureCount(type);

    for (GLuint i = 0; i < texturesCount; ++i)
    {
        aiString str;
        mat->GetTexture(type, i, &str);

        Texture * texture = new Texture();
        texture->setTypeName(type_name);
        texture->createTexture2D(str.C_Str());
        textures.push_back(texture);
    }

    return textures;
}

void Model::render()
{
    for (auto & mesh : meshes)
    {
        mesh->render();
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
}

void Model::setTexture(std::string filename)
{
    if (meshes.size() > 0 && model_type == VE_PRIMITIVE)
    {
        Texture * t = new Texture();
        t->createTexture2D(filename);
        meshes[0]->textures.push_back(t);
    }
}
