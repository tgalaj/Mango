#include "mgpch.h"

#include "AssimpMeshImporter.h"
#include "Mango/Core/AssetManager.h"
#include "Mango/Core/Services.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

namespace mango
{
    ref<Mesh> AssimpMeshImporter::load(const std::string& filename)
    {
        MG_PROFILE_ZONE_SCOPED;

        ref<Mesh> loadedMesh = createRef<Mesh>(filename);

        auto filepath = VFI::getFilepath(filename);

        /* Load model */
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(filepath.string(), aiProcess_Triangulate              |
                                                                    aiProcess_GenSmoothNormals         |
                                                                    aiProcess_GenUVCoords              |
                                                                    aiProcess_CalcTangentSpace         |
                                                                    aiProcess_FlipUVs                  |
                                                                    aiProcess_JoinIdenticalVertices    |
                                                                    aiProcess_RemoveRedundantMaterials |
                                                                    aiProcess_ImproveCacheLocality     |
                                                                    aiProcess_GenBoundingBoxes);

        if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            MG_CORE_ERROR("AssimpMeshImporter: error while loading mesh {}\n Error: {}", filepath.string(), importer.GetErrorString());
            return nullptr;
        }

        auto parentDirectory = std::filesystem::path(filename).parent_path();

        if (!parseScene(loadedMesh, scene, parentDirectory))
        {
            return nullptr;
        }

        return loadedMesh;
    }

    bool AssimpMeshImporter::parseScene(ref<Mesh>& mesh, const aiScene* scene, const std::filesystem::path& parentDirectory)
    {
        MG_PROFILE_ZONE_SCOPED;
        mesh->m_submeshes.resize(scene->mNumMeshes);

        Mesh::VertexData vertexData    = {};
        uint32_t         verticesCount = 0;
        uint32_t         indicesCount  = 0;

        /* Count the number of vertices and indices. */
        for (uint32_t i = 0; i < mesh->m_submeshes.size(); ++i)
        {
            mesh->m_submeshes[i].materialIndex = scene->mMeshes[i]->mMaterialIndex;
            mesh->m_submeshes[i].indicesCount  = scene->mMeshes[i]->mNumFaces * 3;
            mesh->m_submeshes[i].baseVertex    = verticesCount;
            mesh->m_submeshes[i].baseIndex     = indicesCount;

            verticesCount += scene->mMeshes[i]->mNumVertices;
            indicesCount  += mesh->m_submeshes[i].indicesCount;
        }

        /* Reserve space in the vectors for the vertex attributes and indices. */
        vertexData.positions.reserve(verticesCount);
        vertexData.texcoords.reserve(verticesCount);
        vertexData.normals.reserve(verticesCount);
        vertexData.tangents.reserve(verticesCount);
        vertexData.indices.reserve(indicesCount);

        /* Load mesh parts one by one. */
        glm::vec3 min = glm::vec3(std::numeric_limits<float>::max());
        glm::vec3 max = -min;

        for (uint32_t i = 0; i < mesh->m_submeshes.size(); ++i)
        {
            auto mesh = scene->mMeshes[i];
            loadMeshPart(mesh, vertexData);

            min = glm::min(min, vec3_cast(mesh->mAABB.mMin));
            max = glm::max(max, vec3_cast(mesh->mAABB.mMax));
        }

        mesh->m_unitScale = 1.0f / glm::compMax(max - min);

        /* Load materials. */
        if (!loadMaterials(mesh, scene, parentDirectory))
        {
            MG_CORE_ERROR("Assimp error while loading mesh {}\n Error: Could not load the materials.", (parentDirectory / mesh->getFilename()).string());
            return false;
        }

        /* Populate buffers on the GPU with the model's data. */
        mesh->createBuffers(vertexData);

        return true;
    }

    void AssimpMeshImporter::loadMeshPart(const aiMesh* mesh, Mesh::VertexData& vertexData)
    {
        MG_PROFILE_ZONE_SCOPED;
        for (uint32_t i = 0; i < mesh->mNumVertices; ++i)
        {
            auto pos      = vec3_cast(mesh->mVertices[i]);
            auto texcoord = mesh->HasTextureCoords(0)        ? vec3_cast(mesh->mTextureCoords[0][i]) : math::zero;
            auto normal   = mesh->HasNormals()               ? vec3_cast(mesh->mNormals[i])          : math::zero;
            auto tangent  = mesh->HasTangentsAndBitangents() ? vec3_cast(mesh->mTangents[i])         : math::zero;

            vertexData.positions.push_back(pos);
            vertexData.texcoords.push_back(glm::vec2(texcoord));
            vertexData.normals.push_back(normal);
            vertexData.tangents.push_back(tangent);
        }

        for (uint32_t i = 0; i < mesh->mNumFaces; ++i)
        {
            const aiFace& face = mesh->mFaces[i];
            MG_CORE_ASSERT(face.mNumIndices == 3);

            for (char i = 0; i < face.mNumIndices; ++i)
            {
                vertexData.indices.push_back(face.mIndices[i]);
            }
        }
    }

    bool AssimpMeshImporter::loadMaterials(ref<Mesh>& mesh, const aiScene* scene, const std::filesystem::path& parentDirectory)
    {
        MG_PROFILE_ZONE_SCOPED;
        bool ret = true;

        if (!scene->HasMaterials())
        {
            return ret;
        }

        if (!mesh->m_materialTable.empty()) mesh->m_materialTable.clear();

        mesh->m_materialTable.resize(scene->mNumMaterials);

        for (uint32_t i = 0; i < scene->mNumMaterials; ++i)
        {
            auto        pMaterial    = scene->mMaterials[i];
            std::string materialName = pMaterial->GetName().C_Str();

            if (materialName.empty())
            {
                materialName = std::filesystem::path(mesh->getFilename()).stem().string() + "_" + std::to_string(i+1);
            }

            auto mangoMaterial = AssetManager::createMaterial(materialName);
            MG_CORE_ERROR("Material name {}", mangoMaterial->name);
            // NOTE(TG): Leaving parts of code commented as we'll need it when moving to PBR workflow
            
            //ret |= loadMaterialTextures(scene, pMaterial, aiTextureType_BASE_COLOR,        Material::TextureType::ALBEDO,    dir);
            ret |= loadMaterialTextures(scene, pMaterial, mangoMaterial, aiTextureType_DIFFUSE,      Material::TextureType::DIFFUSE,      parentDirectory);
            ret |= loadMaterialTextures(scene, pMaterial, mangoMaterial, aiTextureType_SPECULAR,     Material::TextureType::SPECULAR,     parentDirectory);
            ret |= loadMaterialTextures(scene, pMaterial, mangoMaterial, aiTextureType_NORMALS,      Material::TextureType::NORMAL,       parentDirectory);
            ret |= loadMaterialTextures(scene, pMaterial, mangoMaterial, aiTextureType_EMISSIVE,     Material::TextureType::EMISSION,     parentDirectory);
            ret |= loadMaterialTextures(scene, pMaterial, mangoMaterial, aiTextureType_DISPLACEMENT, Material::TextureType::DISPLACEMENT, parentDirectory);
            //ret |= loadMaterialTextures(scene, pMaterial, aiTextureType_AMBIENT_OCCLUSION, Material::TextureType::AO,        dir);
            //ret |= loadMaterialTextures(scene, pMaterial, aiTextureType_DIFFUSE_ROUGHNESS, Material::TextureType::ROUGHNESS, dir);
            //ret |= loadMaterialTextures(scene, pMaterial, aiTextureType_METALNESS,         Material::TextureType::METALLIC,  dir);

            /* Load material parameters */
            // aiColor3D colorRgb;
            // aiColor4D colorRgba;
            float value;

            // if (AI_SUCCESS == pMaterial->Get(AI_MATKEY_BASE_COLOR, color_rgba))
            // {
            //     mangoMaterial->addVector3("u_albedo", glm::vec3(color_rgba.r, color_rgba.g, color_rgba.b));
            // }
            // if (AI_SUCCESS == pMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, color_rgb))
            // {
            //     mangoMaterial->addVector3("u_emission", glm::vec3(color_rgb.r, color_rgb.g, color_rgb.b));
            // }
            // if (AI_SUCCESS == pMaterial->Get(AI_MATKEY_COLOR_AMBIENT, color_rgb))
            // {
            //     mangoMaterial->addFloat("u_ao", (color_rgb.r + color_rgb.g + color_rgb.b) / 3.0f);
            // }
            // if (AI_SUCCESS == pMaterial->Get(AI_MATKEY_ROUGHNESS_FACTOR, value))
            // {
            //     mangoMaterial->addFloat("u_roughness", value);
            // }
            // if (AI_SUCCESS == pMaterial->Get(AI_MATKEY_METALLIC_FACTOR, value))
            // {
            //     mangoMaterial->addFloat("u_metallic", value);
            // }

            if (AI_SUCCESS == pMaterial->Get(AI_MATKEY_SHININESS, value))
            {
                mangoMaterial->addFloat("specular_power", value);
            }
            if (AI_SUCCESS == pMaterial->Get(AI_MATKEY_SHININESS_STRENGTH, value))
            {
                mangoMaterial->addFloat("specular_intensity", value);
            }
            if (AI_SUCCESS == pMaterial->Get(AI_MATKEY_OPACITY, value))
            {
                if (value != 1.0f)
                {
                    mangoMaterial->setRenderQueue(Material::RenderQueue::RQ_TRANSPARENT);
                    mangoMaterial->addFloat("alpha", value);
                }
            }

            mesh->m_materialTable[i] = mangoMaterial;
        }

        return ret;
    }

    bool AssimpMeshImporter::loadMaterialTextures(const aiScene* scene, const aiMaterial* material, ref<Material>& mangoMaterial, aiTextureType aiType, Material::TextureType textureType, const std::filesystem::path& parentDirectory)
    {
        MG_PROFILE_ZONE_SCOPED;
        if (material->GetTextureCount(aiType) > 0)
        {
            aiString path;
            aiTextureMapMode textureMapMode[3];
            
            // Only one texture of a given type is being loaded
            if (material->GetTexture(aiType, 0, &path, NULL, NULL, NULL, NULL, textureMapMode) == AI_SUCCESS)
            {
                bool isSrgb = (aiType == aiTextureType_DIFFUSE) || (aiType == aiTextureType_EMISSIVE) || (aiType == aiTextureType_BASE_COLOR);

                      ref<Texture> texture    = createRef<Texture>();
                const aiTexture*   paiTexture = scene->GetEmbeddedTexture(path.C_Str());

                if (paiTexture)
                {
                    // Load embedded
                    uint32_t dataSize = paiTexture->mHeight > 0 ? paiTexture->mWidth * paiTexture->mHeight : paiTexture->mWidth;
                    
                    if (texture->createTexture2dFromMemory(reinterpret_cast<unsigned char*>(paiTexture->pcData), dataSize, isSrgb))
                    {
                        MG_CORE_TRACE("Loaded embedded texture for the model {}", path.C_Str());
                        mangoMaterial->addTexture(textureType, texture);

                        if (textureMapMode[0] == aiTextureMapMode_Wrap)
                        {
                            texture->setWraping(TextureWrapingCoordinate::S, TextureWrapingParam::REPEAT);
                            texture->setWraping(TextureWrapingCoordinate::T, TextureWrapingParam::REPEAT);
                        }
                    }
                    else
                    {
                        MG_CORE_ERROR("Error loading embedded texture for the model {}.", path.C_Str());
                        return false;
                    }
                }
                else
                {
                    // Load from file
                    auto fullPath = parentDirectory / path.C_Str();
                    if (!texture->createTexture2d(fullPath.string(), isSrgb))
                    {
                        MG_CORE_ERROR("Error loading texture {}.", fullPath);
                        return false;
                    }
                    
                    
                    MG_CORE_TRACE("Loaded texture {}.", fullPath);
                    mangoMaterial->addTexture(textureType, texture);

                    if (textureMapMode[0] == aiTextureMapMode_Wrap)
                    {
                        texture->setWraping(TextureWrapingCoordinate::S, TextureWrapingParam::REPEAT);
                        texture->setWraping(TextureWrapingCoordinate::T, TextureWrapingParam::REPEAT);
                    }
                    
                }
            }
        }
        return true;
    }
}