#include "mgpch.h"

#include "StaticMesh.h"
#include "Mango/Core/AssetManager.h"
#include "Mango/Core/Services.h"

#include <assimp/postprocess.h>
#include <glm/gtc/matrix_transform.hpp>

namespace mango
{
    void StaticMesh::render(uint32_t instancesCount)
    {
        MG_PROFILE_ZONE_SCOPED;
        glBindVertexArray(m_vaoName);

        for (uint32_t i = 0; i < m_submeshes.size(); i++)
        {
            auto& material = m_submeshes[i].material;
            if (material)
            {
                for (auto const& [texture_type, texture] : material->getTexturesMap())
                {
                    texture->bind(uint32_t(texture_type));
                }
            }

            if (instancesCount == 0)
            {
                glDrawElementsBaseVertex(GLenum(m_drawMode),
                                         m_submeshes[i].indicesCount,
                                         GL_UNSIGNED_INT,
                                         (void*)(sizeof(uint32_t) * m_submeshes[i].baseIndex),
                                         m_submeshes[i].baseVertex);
            }
            else
            {
                glDrawElementsInstancedBaseVertex(GLenum(m_drawMode),
                                                  m_submeshes[i].indicesCount,
                                                  GL_UNSIGNED_INT,
                                                  (void*)(sizeof(uint32_t) * m_submeshes[i].baseIndex),
                                                  instancesCount,
                                                  m_submeshes[i].baseVertex);
            }
        }

        glBindTextureUnit(0, 0);
    }

    void StaticMesh::render(ref<Shader>& shader, uint32_t instancesCount)
    {
        MG_PROFILE_ZONE_SCOPED;
        glBindVertexArray(m_vaoName);
    
        for (uint32_t i = 0 ; i < m_submeshes.size() ; i++) 
        {
            auto& material = m_submeshes[i].material;
            if (material)
            {
                for (auto const& [texture_type, texture] : material->getTexturesMap())
                {
                    texture->bind(uint32_t(texture_type));
                }

                // Set uniforms based on the data in the material
                for (auto& [uniform_name, value] : material->getBoolMap())
                {
                    shader->setUniform(uniform_name, value);
                }

                for (auto& [uniform_name, value] : material->getFloatMap())
                {
                    shader->setUniform(uniform_name, value);
                }

                for (auto& [uniform_name, value] : material->getVec3Map())
                {
                    shader->setUniform(uniform_name, value);
                }
            }

            if (instancesCount == 0 )
            {
                glDrawElementsBaseVertex(GLenum(m_drawMode), 
                                         m_submeshes[i].indicesCount,
                                         GL_UNSIGNED_INT,
                                         (void*)(sizeof(unsigned int) * m_submeshes[i].baseIndex),
                                         m_submeshes[i].baseVertex);
            }
            else
            {
                glDrawElementsInstancedBaseVertex(GLenum(m_drawMode),
                                                  m_submeshes[i].indicesCount,
                                                  GL_UNSIGNED_INT,
                                                  (void*)(sizeof(unsigned int) * m_submeshes[i].baseIndex),
                                                  instancesCount,
                                                  m_submeshes[i].baseVertex);
            }
        }

        glBindTextureUnit(0, 0);
    }

    bool StaticMesh::load(const std::string& filename)
    {
        MG_PROFILE_ZONE_SCOPED;
        /* Release the previously loaded mesh if it was loaded. */
        if (m_vaoName)
        {
            release();
        }

        m_modelType = MeshType::StaticMesh;
        m_filename  = filename;

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
                                                                    aiProcess_GenBoundingBoxes );

        if (!scene || scene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            MG_CORE_ERROR("Assimp error while loading mesh {}\n Error: {}", filepath.string(), importer.GetErrorString());
            return false;
        }

        auto parentDirectory = std::filesystem::path(filename).parent_path();

        return parseScene(scene, parentDirectory);
    }

    bool StaticMesh::parseScene(const aiScene* scene, const std::filesystem::path& parentDirectory)
    {
        MG_PROFILE_ZONE_SCOPED;
        m_submeshes.resize(scene->mNumMeshes);

        for (uint32_t i = 0; i < m_submeshes.size(); ++i)
        {
            m_submeshes[i].material = std::make_shared<Material>();
        }

        VertexData vertexData;

        uint32_t verticesCount = 0;
        uint32_t indicesCount  = 0;

        /* Count the number of vertices and indices. */
        for (uint32_t i = 0; i < m_submeshes.size(); ++i)
        {
            m_submeshes[i].indicesCount = scene->mMeshes[i]->mNumFaces * 3;
            m_submeshes[i].baseVertex   = verticesCount;
            m_submeshes[i].baseIndex    = indicesCount;

            verticesCount += scene->mMeshes[i]->mNumVertices;
            indicesCount  += m_submeshes[i].indicesCount;
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

        for (uint32_t i = 0; i < m_submeshes.size(); ++i)
        {
            auto mesh = scene->mMeshes[i];
            loadMeshPart(mesh, vertexData);

            min = glm::min(min, vec3_cast(mesh->mAABB.mMin));
            max = glm::max(max, vec3_cast(mesh->mAABB.mMax));
        }

        m_unitScale = 1.0f / glm::compMax(max - min);

        /* Load materials. */
        if (!loadMaterials(scene, parentDirectory))
        {
            MG_CORE_ERROR("Assimp error while loading mesh {}\n Error: Could not load the materials.", (parentDirectory / m_filename).string());
            return false;
        }

        /* Populate buffers on the GPU with the model's data. */
        createBuffers(vertexData);

        return true;
    }

    void StaticMesh::loadMeshPart(const aiMesh* mesh, VertexData& vertexData)
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

    bool StaticMesh::loadMaterials(const aiScene* scene, const std::filesystem::path& parentDirectory)
    {
        MG_PROFILE_ZONE_SCOPED;
        bool ret = true;

        if (!scene->HasMaterials())
        {
            return ret;
        }

        std::vector<ref<Material>> materials(scene->mNumMaterials);

        for (uint32_t i = 0; i < scene->mNumMaterials; ++i)
        {
            auto        pMaterial    = scene->mMaterials[i];
            std::string materialName = pMaterial->GetName().C_Str();

            if (materialName.empty())
            {
                materialName = std::filesystem::path(m_filename).stem().string() + "_" + std::to_string(i+1);
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

            materials[i] = mangoMaterial;
        }

        // Assign a proper material for each submesh based on the mesh's material index
        for (uint32_t i = 0; i < m_submeshes.size(); ++i)
        {
            uint32_t materialIndex  = scene->mMeshes[i]->mMaterialIndex;
            m_submeshes[i].material = materials[materialIndex];
        }

        return ret;
    }

    bool StaticMesh::loadMaterialTextures(const aiScene* scene, const aiMaterial* material, ref<Material>& mangoMaterial, aiTextureType type, Material::TextureType textureType, const std::filesystem::path& parentDirectory) const
    {
        MG_PROFILE_ZONE_SCOPED;
        if (material->GetTextureCount(type) > 0)
        {
            aiString path;
            aiTextureMapMode textureMapMode[3];
            
            // Only one texture of a given type is being loaded
            if (material->GetTexture(type, 0, &path, NULL, NULL, NULL, NULL, textureMapMode) == AI_SUCCESS)
            {
                bool isSrgb = (type == aiTextureType_DIFFUSE) || (type == aiTextureType_EMISSIVE) || (type == aiTextureType_BASE_COLOR);

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

    void StaticMesh::createBuffers(VertexData& vertexData)
{
        MG_PROFILE_ZONE_SCOPED;
        bool hasTangents = !vertexData.tangents.empty();

        const GLsizei positionsSizeBytes = vertexData.positions.size() * sizeof(vertexData.positions[0]);
        const GLsizei texcoordsSizeBytes = vertexData.texcoords.size() * sizeof(vertexData.texcoords[0]);
        const GLsizei normalsSizeBytes   = vertexData.normals  .size() * sizeof(vertexData.normals  [0]);
        const GLsizei tangentsSizeBytes  = hasTangents ? vertexData.tangents .size() * sizeof(vertexData.tangents [0]) : 0;
        const GLsizei totalSizeBytes     = positionsSizeBytes + texcoordsSizeBytes + normalsSizeBytes + tangentsSizeBytes;

        glCreateBuffers     (1, &m_vboName);
        glNamedBufferStorage(m_vboName, totalSizeBytes, nullptr, GL_DYNAMIC_STORAGE_BIT);

        uint64_t offset = 0;
        glNamedBufferSubData(m_vboName, offset, positionsSizeBytes, vertexData.positions.data());

        offset += positionsSizeBytes;
        glNamedBufferSubData(m_vboName, offset, texcoordsSizeBytes, vertexData.texcoords.data());

        offset += texcoordsSizeBytes;
        glNamedBufferSubData(m_vboName, offset, normalsSizeBytes, vertexData.normals.data());

        if(hasTangents)
        {
            offset += normalsSizeBytes;
            glNamedBufferSubData(m_vboName, offset, tangentsSizeBytes, vertexData.tangents.data());
        }

        glCreateBuffers     (1, &m_iboName);
        glNamedBufferStorage(m_iboName, sizeof(vertexData.indices[0]) * vertexData.indices.size(), vertexData.indices.data(), GL_DYNAMIC_STORAGE_BIT);

        glCreateVertexArrays(1, &m_vaoName);

        offset = 0;
        glVertexArrayVertexBuffer(m_vaoName, 0 /* bindingindex*/, m_vboName, offset, sizeof(vertexData.positions[0]) /*stride*/);
                          
        offset += positionsSizeBytes;
        glVertexArrayVertexBuffer(m_vaoName, 1 /* bindingindex*/, m_vboName, offset, sizeof(vertexData.texcoords[0]) /*stride*/);
        
        offset += texcoordsSizeBytes;
        glVertexArrayVertexBuffer(m_vaoName, 2 /* bindingindex*/, m_vboName,  offset, sizeof(vertexData.normals[0]) /*stride*/);

        if (hasTangents)
        {
            offset += normalsSizeBytes;
            glVertexArrayVertexBuffer(m_vaoName, 3 /* bindingindex*/, m_vboName, offset, sizeof(vertexData.tangents[0]) /*stride*/);
        }

        glVertexArrayElementBuffer(m_vaoName, m_iboName);

                         glEnableVertexArrayAttrib(m_vaoName, 0 /*attribindex*/); // positions
                         glEnableVertexArrayAttrib(m_vaoName, 1 /*attribindex*/); // texcoords
                         glEnableVertexArrayAttrib(m_vaoName, 2 /*attribindex*/); // normals
        if (hasTangents) glEnableVertexArrayAttrib(m_vaoName, 3 /*attribindex*/); // tangents

                         glVertexArrayAttribFormat(m_vaoName, 0 /*attribindex */, 3 /* size */, GL_FLOAT, GL_FALSE, 0 /*relativeoffset*/); 
                         glVertexArrayAttribFormat(m_vaoName, 1 /*attribindex */, 2 /* size */, GL_FLOAT, GL_FALSE, 0 /*relativeoffset*/); 
                         glVertexArrayAttribFormat(m_vaoName, 2 /*attribindex */, 3 /* size */, GL_FLOAT, GL_FALSE, 0 /*relativeoffset*/); 
        if (hasTangents) glVertexArrayAttribFormat(m_vaoName, 3 /*attribindex */, 3 /* size */, GL_FLOAT, GL_FALSE, 0 /*relativeoffset*/);

                         glVertexArrayAttribBinding(m_vaoName, 0 /*attribindex*/, 0 /*bindingindex*/); // positions
                         glVertexArrayAttribBinding(m_vaoName, 1 /*attribindex*/, 1 /*bindingindex*/); // texcoords
                         glVertexArrayAttribBinding(m_vaoName, 2 /*attribindex*/, 2 /*bindingindex*/); // normals
        if (hasTangents) glVertexArrayAttribBinding(m_vaoName, 3 /*attribindex*/, 3 /*bindingindex*/); // tangents
    }

    /* The first available input attribute index is 4. */
    void StaticMesh::addAttributeBuffer(GLuint attribIndex, GLuint bindingIndex, GLint formatSize, GLenum dataType, GLuint bufferID, GLsizei stride, GLuint divisor)
    {
        MG_PROFILE_ZONE_SCOPED;
        if (m_vaoName)
        {
            glVertexArrayVertexBuffer  (m_vaoName, bindingIndex, bufferID, 0 /*offset*/, stride);
            glEnableVertexArrayAttrib  (m_vaoName, attribIndex);
            glVertexArrayAttribFormat  (m_vaoName, attribIndex, formatSize, dataType, GL_FALSE, 0 /*relativeoffset*/);
            glVertexArrayAttribBinding (m_vaoName, attribIndex, bindingIndex);
            glVertexArrayBindingDivisor(m_vaoName, bindingIndex, divisor);
        }
    }

    void StaticMesh::setMaterial(ref<Material>& material, uint32_t meshID)
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_CORE_ASSERT(meshID < m_submeshes.size());
        m_submeshes[meshID].material = material;
    }

    void StaticMesh::calcTangentSpace(VertexData& vertexData)
    {
        MG_PROFILE_ZONE_SCOPED;

        vertexData.tangents.resize(vertexData.positions.size());
        std::fill(std::begin(vertexData.tangents), std::end(vertexData.tangents), glm::vec3(0.0f));

        for(uint32_t i = 0; i < vertexData.indices.size(); i += 3)
        {
            auto i0 = vertexData.indices[i];
            auto i1 = vertexData.indices[i + 1];
            auto i2 = vertexData.indices[i + 2];

            auto edge1 = vertexData.positions[i1] - vertexData.positions[i0];
            auto edge2 = vertexData.positions[i2] - vertexData.positions[i0];

            auto deltaUV1 = vertexData.texcoords[i1] - vertexData.texcoords[i0];
            auto deltaUV2 = vertexData.texcoords[i2] - vertexData.texcoords[i0];

            float dividend = (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
            float f = dividend == 0.0f ? 0.0f : 1.0f / dividend;

            glm::vec3 tangent(0.0f);
            tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
            tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
            tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

            vertexData.tangents[i0] += tangent;
            vertexData.tangents[i1] += tangent;
            vertexData.tangents[i2] += tangent;
        }

        for (uint32_t i = 0; i < vertexData.positions.size(); ++i)
        {
            vertexData.tangents[i] = glm::normalize(vertexData.tangents[i]);
        }
    }

    void StaticMesh::genPrimitive(VertexData& vertexData, bool generateTangents /*= true*/)
    {
        MG_PROFILE_ZONE_SCOPED;

        /* Release the previously loaded mesh if it was loaded. */
        if (m_vaoName)
        {
            release();
        }

        if (generateTangents)
        {
            calcTangentSpace(vertexData);
        }

        createBuffers(vertexData);
        
        Submesh submesh;
        submesh.baseIndex    = 0;
        submesh.baseVertex   = 0;
        submesh.indicesCount = vertexData.indices.size();
        submesh.material     = AssetManager::createMaterial("default");

        m_submeshes.push_back(submesh);
    }

    void StaticMesh::genCone(float height, float radius, uint32_t slices, uint32_t stacks)
    {
        MG_PROFILE_ZONE_SCOPED;

        m_modelType = MeshType::Cone;

        m_primitiveProperties =
        {
            .height = height <= 0.0f ? 0.01f : height,
            .radius = radius <= 0.0f ? 0.01f : radius,
            .slices = (int32_t)slices < 3 ? 3 : (int32_t)slices,
            .stacks = (int32_t)stacks < 1 ? 1 : (int32_t)stacks
        };

        VertexData vertexData;

        float thetaInc = glm::two_pi<float>() / float(slices);
        float theta    = 0.0f;

        /* Center bottom */
        glm::vec3 p = glm::vec3(0.0f, height, 0.0f);

        vertexData.positions.push_back(-p);
        vertexData.normals  .push_back(glm::vec3(0.0f, -1.0f, 0.0f));
        vertexData.texcoords.push_back(glm::vec2(0.5f, 0.5f));

        /* Bottom */
        for (uint32_t sideCount = 0; sideCount <= slices; ++sideCount, theta += thetaInc)
        {
            vertexData.positions.push_back(glm::vec3(glm::cos(theta) * radius, -height, -glm::sin(theta) * radius));
            vertexData.normals  .push_back(glm::vec3(0.0f, -1.0f, 0.0f));
            vertexData.texcoords.push_back(glm::vec2(glm::cos(theta) * 0.5f + 0.5f, glm::sin(theta) * 0.5f + 0.5f));
        }

        /* Sides */
        float l = glm::sqrt(height * height + radius * radius);

        for (uint32_t stackCount = 0; stackCount <= stacks; ++stackCount)
        {
            float level = stackCount / float(stacks);

            for (uint32_t sliceCount = 0; sliceCount <= slices; ++sliceCount, theta += thetaInc)
            {
                vertexData.positions.push_back( glm::vec3(glm::cos(theta) * radius * (1.0f - level),
                                                -height + height * level,
                                                -glm::sin(theta) * radius * (1.0f - level)));
                vertexData.normals  .push_back( glm::vec3(glm::cos(theta) * height / l, radius / l, -glm::sin(theta) * height / l));
                vertexData.texcoords.push_back( glm::vec2(sliceCount / float(slices), level));
            }
        }

        uint32_t centerIdx = 0;
        uint32_t idx       = 1;

        /* Indices Bottom */
        for (uint32_t sliceCount = 0; sliceCount < slices; ++sliceCount)
        {
            vertexData.indices.push_back(centerIdx);
            vertexData.indices.push_back(idx + 1);
            vertexData.indices.push_back(idx);

            ++idx;
        }
        ++idx;

        /* Indices Sides */
        for (uint32_t stackCount = 0; stackCount < stacks; ++stackCount)
        {
            for (uint32_t sliceCount = 0; sliceCount < slices; ++sliceCount)
            {
                vertexData.indices.push_back(idx);
                vertexData.indices.push_back(idx + 1);
                vertexData.indices.push_back(idx + slices + 1);

                vertexData.indices.push_back(idx + 1);
                vertexData.indices.push_back(idx + slices + 2);
                vertexData.indices.push_back(idx + slices + 1);

                ++idx;
            }

            ++idx;
        }

        genPrimitive(vertexData);
    }

    void StaticMesh::genCube(float size, float texcoordScale)
    {
        MG_PROFILE_ZONE_SCOPED;

        m_modelType = MeshType::Cube;

        m_primitiveProperties =
        {
            .size = size <= 0.0f ? 0.01f : size
        };

        VertexData vertexData;

        float r2 = size;

        vertexData.positions =
        {
            glm::vec3(-r2, -r2, -r2),
            glm::vec3(-r2, -r2, +r2),
            glm::vec3(+r2, -r2, +r2),
            glm::vec3(+r2, -r2, -r2),

            glm::vec3(-r2, +r2, -r2),
            glm::vec3(-r2, +r2, +r2),
            glm::vec3(+r2, +r2, +r2),
            glm::vec3(+r2, +r2, -r2),

            glm::vec3(-r2, -r2, -r2),
            glm::vec3(-r2, +r2, -r2),
            glm::vec3(+r2, +r2, -r2),
            glm::vec3(+r2, -r2, -r2),

            glm::vec3(-r2, -r2, +r2),
            glm::vec3(-r2, +r2, +r2),
            glm::vec3(+r2, +r2, +r2),
            glm::vec3(+r2, -r2, +r2),

            glm::vec3(-r2, -r2, -r2),
            glm::vec3(-r2, -r2, +r2),
            glm::vec3(-r2, +r2, +r2),
            glm::vec3(-r2, +r2, -r2),

            glm::vec3(+r2, -r2, -r2),
            glm::vec3(+r2, -r2, +r2),
            glm::vec3(+r2, +r2, +r2),
            glm::vec3(+r2, +r2, -r2) 
        };

        vertexData.normals =
        {
            glm::vec3(0.0f, -1.0f, 0.0f),
            glm::vec3(0.0f, -1.0f, 0.0f),
            glm::vec3(0.0f, -1.0f, 0.0f),
            glm::vec3(0.0f, -1.0f, 0.0f),
            
            glm::vec3(0.0f, +1.0f, 0.0f),
            glm::vec3(0.0f, +1.0f, 0.0f),
            glm::vec3(0.0f, +1.0f, 0.0f),
            glm::vec3(0.0f, +1.0f, 0.0f),
            
            glm::vec3(0.0f, 0.0f, -1.0f),
            glm::vec3(0.0f, 0.0f, -1.0f),
            glm::vec3(0.0f, 0.0f, -1.0f),
            glm::vec3(0.0f, 0.0f, -1.0f),
            
            glm::vec3(0.0f, 0.0f, +1.0f),
            glm::vec3(0.0f, 0.0f, +1.0f),
            glm::vec3(0.0f, 0.0f, +1.0f),
            glm::vec3(0.0f, 0.0f, +1.0f),
            
            glm::vec3(-1.0f, 0.0f, 0.0f),
            glm::vec3(-1.0f, 0.0f, 0.0f),
            glm::vec3(-1.0f, 0.0f, 0.0f),
            glm::vec3(-1.0f, 0.0f, 0.0f),
            
            glm::vec3(+1.0f, 0.0f, 0.0f),
            glm::vec3(+1.0f, 0.0f, 0.0f),
            glm::vec3(+1.0f, 0.0f, 0.0f),
            glm::vec3(+1.0f, 0.0f, 0.0f)
        };

        vertexData.texcoords =
        {
            glm::vec2(0.0f, 0.0f) * texcoordScale,
            glm::vec2(0.0f, 1.0f) * texcoordScale,
            glm::vec2(1.0f, 1.0f) * texcoordScale,
            glm::vec2(1.0f, 0.0f) * texcoordScale,
                    
            glm::vec2(0.0f, 1.0f) * texcoordScale,
            glm::vec2(0.0f, 0.0f) * texcoordScale,
            glm::vec2(1.0f, 0.0f) * texcoordScale,
            glm::vec2(1.0f, 1.0f) * texcoordScale,
                    
            glm::vec2(1.0f, 0.0f) * texcoordScale,
            glm::vec2(1.0f, 1.0f) * texcoordScale,
            glm::vec2(0.0f, 1.0f) * texcoordScale,
            glm::vec2(0.0f, 0.0f) * texcoordScale,

            glm::vec2(0.0f, 0.0f) * texcoordScale,
            glm::vec2(0.0f, 1.0f) * texcoordScale,
            glm::vec2(1.0f, 1.0f) * texcoordScale,
            glm::vec2(1.0f, 0.0f) * texcoordScale,

            glm::vec2(0.0f, 0.0f) * texcoordScale,
            glm::vec2(1.0f, 0.0f) * texcoordScale,
            glm::vec2(1.0f, 1.0f) * texcoordScale,
            glm::vec2(0.0f, 1.0f) * texcoordScale,

            glm::vec2(1.0f, 0.0f) * texcoordScale,
            glm::vec2(0.0f, 0.0f) * texcoordScale,
            glm::vec2(0.0f, 1.0f) * texcoordScale,
            glm::vec2(1.0f, 1.0f) * texcoordScale
        };

        vertexData.indices = 
        {
            0,  2,  1,  0,  3,  2,
            4,  5,  6,  4,  6,  7,
            8,  9,  10, 8,  10, 11,
            12, 15, 14, 12, 14, 13,
            16, 17, 18, 16, 18, 19,
            20, 23, 22, 20, 22, 21
        };

        genPrimitive(vertexData);
    }

    void StaticMesh::genCubeMap(float radius)
    {
        MG_PROFILE_ZONE_SCOPED;
        VertexData vertexData;

        float r2 = radius * 0.5f;

        vertexData.positions =
        {
            // Front
            glm::vec3(-r2, -r2, +r2),
            glm::vec3(+r2, -r2, +r2),
            glm::vec3(+r2, +r2, +r2),
            glm::vec3(-r2, +r2, +r2),
            // Right
            glm::vec3(+r2, -r2, +r2),
            glm::vec3(+r2, -r2, -r2),
            glm::vec3(+r2, +r2, -r2),
            glm::vec3(+r2, +r2, +r2),
            // Back
            glm::vec3(-r2, -r2, -r2),
            glm::vec3(-r2, +r2, -r2),
            glm::vec3(+r2, +r2, -r2),
            glm::vec3(+r2, -r2, -r2),
            // Left
            glm::vec3(-r2, -r2, +r2),
            glm::vec3(+r2, +r2, +r2),
            glm::vec3(-r2, +r2, -r2),
            glm::vec3(-r2, -r2, -r2),
            // Bottom
            glm::vec3(-r2, -r2, +r2),
            glm::vec3(-r2, -r2, -r2),
            glm::vec3(+r2, -r2, -r2),
            glm::vec3(+r2, -r2, +r2),
            // Top
            glm::vec3(-r2, +r2, +r2),
            glm::vec3(+r2, +r2, +r2),
            glm::vec3(+r2, +r2, -r2),
            glm::vec3(+r2, -r2, +r2)
        };

        vertexData.indices =
        {
            0,  2,  1,  0,  3,  2,
            4,  6,  5,  4,  7,  6,
            8,  10, 9,  8,  11, 10,
            12, 14, 13, 12, 15, 14,
            16, 18, 17, 16, 19, 18,
            20, 22, 21, 20, 23, 22
        };

        genPrimitive(vertexData);
    }

    void StaticMesh::genCylinder(float height, float radius, uint32_t slices)
    {
        MG_PROFILE_ZONE_SCOPED;

        m_modelType = MeshType::Cylinder;

        m_primitiveProperties =
        {
            .height = height <= 0.0f ? 0.01f : height,
            .radius = radius <= 0.0f ? 0.01f : radius,
            .slices = (int32_t)slices < 3 ? 3 : (int32_t)slices
        };

        VertexData vertexData;

        float halfHeight = height * 0.5f;
        glm::vec3 p1 = glm::vec3(0.0f, halfHeight, 0.0f);
        glm::vec3 p2 = -p1;

        float thetaInc = glm::two_pi<float>() / float(slices);
        float theta = 0.0f;
        float sign = -1.0f;

        /* Center bottom */
        vertexData.positions.push_back(p2);
        vertexData.normals  .push_back(glm::vec3(0.0f, -1.0f, 0.0f));
        vertexData.texcoords.push_back(glm::vec2(0.5f, 0.5f));

        /* Bottom */
        for (uint32_t sideCount = 0; sideCount <= slices; ++sideCount, theta += thetaInc)
        {
            vertexData.positions.push_back(glm::vec3(glm::cos(theta) * radius, -halfHeight, -glm::sin(theta) * radius));
            vertexData.normals  .push_back(glm::vec3(0.0f, -1.0f, 0.0f));
            vertexData.texcoords.push_back(glm::vec2(glm::cos(theta) * 0.5f + 0.5f, glm::sin(theta) * 0.5f + 0.5f));
        }

        /* Center top */
        vertexData.positions.push_back(p1);
        vertexData.normals  .push_back(glm::vec3(0.0f, 1.0f, 0.0f));
        vertexData.texcoords.push_back(glm::vec2(0.5f, 0.5f));

        /* Top */
        for (uint32_t sideCount = 0; sideCount <= slices; ++sideCount, theta += thetaInc)
        {
            vertexData.positions.push_back(glm::vec3(glm::cos(theta) * radius, halfHeight, -glm::sin(theta) * radius));
            vertexData.normals  .push_back(glm::vec3(0.0f, 1.0f, 0.0f));
            vertexData.texcoords.push_back(glm::vec2(glm::cos(theta) * 0.5f + 0.5f, glm::sin(theta) * 0.5f + 0.5f));
        }

        /* Sides */
        for (uint32_t sideCount = 0; sideCount <= slices; ++sideCount, theta += thetaInc)
        {
            sign = -1.0f;

            for (int i = 0; i < 2; ++i)
            {
                vertexData.positions.push_back(glm::vec3(glm::cos(theta) * radius, halfHeight * sign, -glm::sin(theta) * radius));
                vertexData.normals  .push_back(glm::vec3(glm::cos(theta), 0.0f, -glm::sin(theta)));
                vertexData.texcoords.push_back(glm::vec2(sideCount / (float)slices, (sign + 1.0f) * 0.5f));

                sign = 1.0f;
            }
        }

        uint32_t centerIdx = 0;
        uint32_t idx = 1;

        /* Indices Bottom */
        for (uint32_t sideCount = 0; sideCount < slices; ++sideCount)
        {
            vertexData.indices.push_back(centerIdx);
            vertexData.indices.push_back(idx + 1);
            vertexData.indices.push_back(idx);

            ++idx;
        }
        ++idx;

        /* Indices Top */
        centerIdx = idx;
        ++idx;

        for (uint32_t sideCount = 0; sideCount < slices; ++sideCount)
        {
            vertexData.indices.push_back(centerIdx);
            vertexData.indices.push_back(idx);
            vertexData.indices.push_back(idx + 1);

            ++idx;
        }
        ++idx;

        /* Indices Sides */
        for (uint32_t sideCount = 0; sideCount < slices; ++sideCount)
        {
            vertexData.indices.push_back(idx);
            vertexData.indices.push_back(idx + 2);
            vertexData.indices.push_back(idx + 1);

            vertexData.indices.push_back(idx + 2);
            vertexData.indices.push_back(idx + 3);
            vertexData.indices.push_back(idx + 1);

            idx += 2;
        }

        genPrimitive(vertexData);
    }

    void StaticMesh::genPlane(float width, float height, uint32_t slices, uint32_t stacks)
    {
        MG_PROFILE_ZONE_SCOPED;

        m_modelType = MeshType::Plane;

        m_primitiveProperties =
        {
            .width = width <= 0.0f ? 0.01f : width,
            .height = height <= 0.0f ? 0.01f : height,
            .slices = (int32_t)slices < 1 ? 1 : (int32_t)slices,
            .stacks = (int32_t)stacks < 1 ? 1 : (int32_t)stacks
        };

        VertexData vertexData;

        float widthInc  = width  / float(slices);
        float heightInc = height / float(stacks);

        float w = -width * 0.5f;
        float h = -height * 0.5f;

        for (uint32_t j = 0; j <= stacks; ++j, h += heightInc)
        {
            for (uint32_t i = 0; i <= slices; ++i, w += widthInc)
            {
                vertexData.positions.push_back(glm::vec3(w, 0.0f, h));
                vertexData.normals  .push_back(glm::vec3(0.0f, 1.0f, 0.0f));
                vertexData.texcoords.push_back(glm::vec2(i, j));
            }
            w = -width * 0.5f;
        }

        uint32_t idx = 0;

        for (uint32_t j = 0; j < stacks; ++j)
        {
            for (uint32_t i = 0; i < slices; ++i)
            {
                vertexData.indices.push_back(idx);
                vertexData.indices.push_back(idx + slices + 1);
                vertexData.indices.push_back(idx + 1);

                vertexData.indices.push_back(idx + 1);
                vertexData.indices.push_back(idx + slices + 1);
                vertexData.indices.push_back(idx + slices + 2);

                ++idx;
            }

            ++idx;
        }

        genPrimitive(vertexData);    
    }

    void StaticMesh::genPlaneGrid(float width, float height, uint32_t slices, uint32_t stacks)
    {
        MG_PROFILE_ZONE_SCOPED;
        m_drawMode = DrawMode::LINES;

        VertexData vertexData;

        float widthInc = width / float(slices);
        float heightInc = height / float(stacks);

        float w = -width * 0.5f;
        float h = -height * 0.5f;

        for (uint32_t j = 0; j <= stacks; ++j, h += heightInc)
        {
            for (uint32_t i = 0; i <= slices; ++i, w += widthInc)
            {
                vertexData.positions.push_back(glm::vec3(w   , 0.0f, h   ));
                vertexData.normals  .push_back(glm::vec3(0.0f, 1.0f, 0.0f));
                vertexData.texcoords.push_back(glm::vec2(i, j));
            }
            w = -width * 0.5f;
        }

        uint32_t idx = 0;

        for (uint32_t j = 0; j < stacks; ++j)
        {
            for (uint32_t i = 0; i < slices; ++i)
            {
                vertexData.indices.push_back(idx);
                vertexData.indices.push_back(idx + 1);

                vertexData.indices.push_back(idx + 1);
                vertexData.indices.push_back(idx + slices + 2);

                vertexData.indices.push_back(idx + slices + 2);
                vertexData.indices.push_back(idx + slices + 1);

                vertexData.indices.push_back(idx + slices + 1);
                vertexData.indices.push_back(idx);

                ++idx;
            }

            ++idx;
        }
        genPrimitive(vertexData, false);
    }

    void StaticMesh::genSphere(float radius, uint32_t slices)
    {
        MG_PROFILE_ZONE_SCOPED;

        m_modelType = MeshType::Sphere;

        m_primitiveProperties =
        {
            .radius = radius <= 0.0f ? 0.01f : radius,
            .slices = (int32_t)slices < 3 ? 3 : (int32_t)slices
        };

        VertexData vertexData;

        float deltaPhi = glm::two_pi<float>() / static_cast<float>(slices);

        uint32_t parallels = static_cast<uint32_t>(slices * 0.5f);

        for (uint32_t i = 0; i <= parallels; ++i)
        {
            for (uint32_t j = 0; j <= slices; ++j)
            {
                vertexData.positions.push_back(glm::vec3(radius * glm::sin(deltaPhi * i) * glm::sin(deltaPhi * j),
                                                          radius * glm::cos(deltaPhi * i),
                                                          radius * glm::sin(deltaPhi * i) * glm::cos(deltaPhi * j)));
                vertexData.normals  .push_back(glm::vec3(radius * glm::sin(deltaPhi * i) * glm::sin(deltaPhi * j) / radius,
                                                          radius * glm::cos(deltaPhi * i) / radius,
                                                          radius * glm::sin(deltaPhi * i) * glm::cos(deltaPhi * j) / radius));
                vertexData.texcoords.push_back(glm::vec2(       j / static_cast<float>(slices),
                                                          1.0f - i / static_cast<float>(parallels)));
            }
        }

        for (uint32_t i = 0; i < parallels; ++i)
        {
            for (uint32_t j = 0; j < slices; ++j)
            {
                vertexData.indices.push_back(i       * (slices + 1) + j);
                vertexData.indices.push_back((i + 1) * (slices + 1) + j);
                vertexData.indices.push_back((i + 1) * (slices + 1) + (j + 1));

                vertexData.indices.push_back(i       * (slices + 1) + j);
                vertexData.indices.push_back((i + 1) * (slices + 1) + (j + 1));
                vertexData.indices.push_back(i       * (slices + 1) + (j + 1));
            }
        }

        genPrimitive(vertexData);
    }

    void StaticMesh::genTorus(float innerRadius, float outerRadius, uint32_t slices, uint32_t stacks)
    {
        MG_PROFILE_ZONE_SCOPED;

        m_modelType = MeshType::Torus;

        m_primitiveProperties =
        {
            .innerRadius = innerRadius <= 0.0f ? 0.01f : innerRadius,
            .outerRadius = outerRadius <= 0.0f ? 0.01f : outerRadius,
            .slices = (int32_t)slices < 3 ? 3 : (int32_t)slices,
            .stacks = (int32_t)stacks < 3 ? 3 : (int32_t)stacks
        };

        VertexData vertexData;

        float phi   = 0.0f;
        float theta = 0.0f;

        float cos2PIp = 0.0f;
        float sin2PIp = 0.0f;
        float cos2PIt = 0.0f;
        float sin2PIt = 0.0f;

        float torusRadius = (outerRadius - innerRadius) * 0.5f;
        float centerRadius = outerRadius - torusRadius;

        float phiInc = 1.0f / float(slices);
        float thetaInc = 1.0f / float(stacks);

        vertexData.positions.reserve((stacks + 1) * (slices + 1));
        vertexData.texcoords.reserve((stacks + 1) * (slices + 1));
        vertexData.normals.reserve((stacks + 1) * (slices + 1));
        vertexData.tangents.reserve((stacks + 1) * (slices + 1));
        vertexData.indices.reserve(stacks * slices * 2 * 3);

        for (uint32_t sideCount = 0; sideCount <= slices; ++sideCount, phi += phiInc)
        {
            cos2PIp = glm::cos(glm::two_pi<float>() * phi);
            sin2PIp = glm::sin(glm::two_pi<float>() * phi);

            theta = 0.0f;
            for (uint32_t faceCount = 0; faceCount <= stacks; ++faceCount, theta += thetaInc)
            {
                cos2PIt = glm::cos(glm::two_pi<float>() * theta);
                sin2PIt = glm::sin(glm::two_pi<float>() * theta);

                vertexData.positions.push_back(glm::vec3((centerRadius + torusRadius * cos2PIt) * cos2PIp,
                                                          (centerRadius + torusRadius * cos2PIt) * sin2PIp,
                                                           torusRadius * sin2PIt));

                vertexData.normals.push_back(glm::vec3(cos2PIp * cos2PIt,
                                                        sin2PIp * cos2PIt,
                                                        sin2PIt));

                vertexData.texcoords.push_back(glm::vec2(phi, theta));
            }
        }

        for (uint32_t sideCount = 0; sideCount < slices; ++sideCount)
        {
            for (uint32_t faceCount = 0; faceCount < stacks; ++faceCount)
            {
                uint32_t v0 = sideCount       * (stacks + 1) + faceCount;
                uint32_t v1 = (sideCount + 1) * (stacks + 1) + faceCount;
                uint32_t v2 = (sideCount + 1) * (stacks + 1) + (faceCount + 1);
                uint32_t v3 = sideCount       * (stacks + 1) + (faceCount + 1);

                vertexData.indices.push_back(v0);
                vertexData.indices.push_back(v1);
                vertexData.indices.push_back(v2);

                vertexData.indices.push_back(v0);
                vertexData.indices.push_back(v2);
                vertexData.indices.push_back(v3);
            }
        }

        genPrimitive(vertexData);
    }

    /* Code courtesy of: https://prideout.net/blog/old/blog/index.html@tag=toon-shader.html */
    void StaticMesh::genTrefoilKnot(uint32_t slices, uint32_t stacks)
    {
        MG_PROFILE_ZONE_SCOPED;

        m_modelType = MeshType::TrefoilKnot;

        m_primitiveProperties =
        {
            .slices = (int32_t)slices < 3 ? 3 : (int32_t)slices,
            .stacks = (int32_t)stacks < 3 ? 3 : (int32_t)stacks
        };

        VertexData vertexData;

        auto evaluate_trefoil = [](float s, float t)
        {
            const float a = 0.5f;
            const float b = 0.3f;
            const float c = 0.5f;
            const float d = 0.1f;
            const float u = (1 - s) * 2 * glm::two_pi<float>();
            const float v = t * glm::two_pi<float>();
            const float r = a + b * cos(1.5f * u);
            const float x = r * cos(u);
            const float y = r * sin(u);
            const float z = c * sin(1.5f * u);

            glm::vec3 dv;
            dv.x = -1.5f * b * sin(1.5f * u) * cos(u) -
                    (a + b * cos(1.5f * u)) * sin(u);
            dv.y = -1.5f * b * sin(1.5f * u) * sin(u) +
                    (a + b * cos(1.5f * u)) * cos(u);
            dv.z =  1.5f * c * cos(1.5f * u);

            glm::vec3 q   = glm::normalize(dv);
            glm::vec3 qvn = glm::normalize(glm::vec3(q.y, -q.x, 0));
            glm::vec3 ww  = glm::cross(qvn, q);

            glm::vec3 range;
            range.x = x + d * (qvn.x * cos(v) + ww.x * sin(v));
            range.y = y + d * (qvn.y * cos(v) + ww.y * sin(v));
            range.z = z + d * ww.z * sin(v);

            return range;
        };

        float ds = 1.0 / slices;
        float dt = 1.0 / stacks;

        const float E = 0.01f;

        // The upper bounds in these loops are tweaked to reduce the
        // chance of precision error causing an incorrect # of iterations.
    
        for (float s = 0; s < 1.0 - ds / 2.0; s += ds)
        {
            for (float t = 0; t < 1.0 - dt / 2.0; t += dt)
            {
                glm::vec3 p = evaluate_trefoil(s, t);
                glm::vec3 u = evaluate_trefoil(s + E, t) - p;
                glm::vec3 v = evaluate_trefoil(s, t + E) - p;
                glm::vec3 n = glm::normalize(glm::cross(v, u));

                vertexData.positions.push_back(p);
                vertexData.normals  .push_back(n);
                vertexData.texcoords.push_back(glm::vec2(s, t));
            }
        }

        uint32_t n            = 0;
        uint32_t vertex_count = vertexData.positions.size();

        for (uint32_t i = 0; i < slices; ++i)
        {
            for (uint32_t j = 0; j < stacks; ++j)
            {
                vertexData.indices.push_back(n + j);
                vertexData.indices.push_back(n + (j + 1) % stacks);
                vertexData.indices.push_back((n + j + stacks) % vertex_count);

                vertexData.indices.push_back((n + j + stacks) % vertex_count);
                vertexData.indices.push_back((n + (j + 1) % stacks) % vertex_count);
                vertexData.indices.push_back((n + (j + 1) % stacks + stacks) % vertex_count);
            }

            n += stacks;
        }

        genPrimitive(vertexData);
    }

    /* Implementation inspired by: https://blackpawn.com/texts/pqtorus/default.html */
    void StaticMesh::genPQTorusKnot(uint32_t slices, uint32_t stacks, int p, int q, float knotR, float tubeR)
    {
        MG_PROFILE_ZONE_SCOPED;

        m_modelType = MeshType::PQTorusKnot;

        m_primitiveProperties =
        {
            .innerRadius = knotR <= 0.0f ? 0.01f : knotR,
            .outerRadius = tubeR <= 0.0f ? 0.01f : tubeR,
            .slices = (int32_t)slices < 3 ? 3 : (int32_t)slices,
            .stacks = (int32_t)stacks < 3 ? 3 : (int32_t)stacks
        };

        VertexData vertexData;

        float theta     = 0.0;
        float thetaStep = glm::two_pi<float>() / slices;

        float phi     = -3.14 / 4.0;
        float phiStep = glm::two_pi<float>() / stacks;

        if (p < 1)
        {
            p = 1;
        }

        if (q < 0)
        {
            q = 0;
        }

        for (uint32_t slice = 0; slice <= slices; ++slice, theta += thetaStep)
        {
            phi = -3.14 / 4.0;

            float r = knotR * (0.5 * (2.0 + glm::sin(q * theta)));
            auto  P = glm::vec3(glm::cos(p * theta), glm::cos(q * theta), glm::sin(p * theta)) * r;

            auto theta_next = theta + thetaStep * 1.0;
                      r     = knotR * (0.5 * (2.0 + glm::sin(q * theta_next)));
            auto pNext      = glm::vec3(glm::cos(p * theta_next), glm::cos(q * theta_next), glm::sin(p * theta_next)) * r;

            auto T = pNext - P;
            auto N = pNext + P;
            auto B = glm::normalize(glm::cross(T, N));
                 N = glm::normalize(glm::cross(B, T)); /* corrected normal */

            for (uint32_t stack = 0; stack <= stacks; ++stack, phi += phiStep)
            {
                glm::vec2 circleVertexPosition = glm::vec2(glm::cos(phi), glm::sin(phi)) * tubeR;

                vertexData.positions.push_back(N * circleVertexPosition.x + B * circleVertexPosition.y + P);
                vertexData.normals  .push_back(glm::normalize(vertexData.positions[stack] - P));
                vertexData.texcoords.push_back(glm::vec2(slice / float(slices), 1.0 - stack / float(stacks)));
            }
        }

        for (uint32_t slice = 0; slice < slices; ++slice)
        {
            for (uint32_t stack = 0; stack < stacks; ++stack)
            {
                uint32_t v0 = slice * (stacks + 1) + stack;
                uint32_t v1 = (slice + 1) * (stacks + 1) + stack;
                uint32_t v2 = (slice + 1) * (stacks + 1) + (stack + 1);
                uint32_t v3 = slice * (stacks + 1) + (stack + 1);
                
                vertexData.indices.push_back(v2);
                vertexData.indices.push_back(v1);
                vertexData.indices.push_back(v0);
                
                vertexData.indices.push_back(v3);
                vertexData.indices.push_back(v2);
                vertexData.indices.push_back(v0);
            }
        }

        genPrimitive(vertexData);
    }

    void StaticMesh::genQuad(float width, float height)
    {
        MG_PROFILE_ZONE_SCOPED;

        m_modelType = MeshType::Quad;
        m_drawMode  = DrawMode::TRIANGLE_STRIP;

        m_primitiveProperties =
        {
            .width = width <= 0.0f ? 0.01f : width,
            .height = height <= 0.0f ? 0.01f : height
        };

        VertexData vertexData;

        float halfWidth  = width * 0.5f;
        float halfHeight = height * 0.5f;

        vertexData.positions.push_back(glm::vec3(-halfWidth, 0.0f, -halfHeight));
        vertexData.positions.push_back(glm::vec3(-halfWidth, 0.0f, halfHeight));
        vertexData.positions.push_back(glm::vec3(halfWidth, 0.0f, -halfHeight));
        vertexData.positions.push_back(glm::vec3(halfWidth, 0.0f, halfHeight));

        vertexData.normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
        vertexData.normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
        vertexData.normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
        vertexData.normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));

        vertexData.texcoords.push_back(glm::vec2(0.0f, 1.0f));
        vertexData.texcoords.push_back(glm::vec2(0.0f, 0.0f));
        vertexData.texcoords.push_back(glm::vec2(1.0f, 1.0f));
        vertexData.texcoords.push_back(glm::vec2(1.0f, 0.0f));

        vertexData.indices.push_back(0);
        vertexData.indices.push_back(1);
        vertexData.indices.push_back(2);
        vertexData.indices.push_back(3);

        genPrimitive(vertexData, false);
    }

    void StaticMesh::regeneratePrimitive()
    {
        switch (m_modelType)
        {
            case mango::StaticMesh::MeshType::Cone:
                genCone(m_primitiveProperties.height, m_primitiveProperties.radius, m_primitiveProperties.slices, m_primitiveProperties.stacks);
                break;
            case mango::StaticMesh::MeshType::Cube:
                genCube(m_primitiveProperties.size);
                break;
            case mango::StaticMesh::MeshType::Cylinder:
                genCylinder(m_primitiveProperties.height, m_primitiveProperties.radius, m_primitiveProperties.slices);
                break;
            case mango::StaticMesh::MeshType::Plane:
                genPlane(m_primitiveProperties.width, m_primitiveProperties.height, m_primitiveProperties.slices, m_primitiveProperties.stacks);
                break;
            case mango::StaticMesh::MeshType::PQTorusKnot:
                genPQTorusKnot(m_primitiveProperties.slices, m_primitiveProperties.stacks, m_primitiveProperties.p, m_primitiveProperties.q, m_primitiveProperties.innerRadius, m_primitiveProperties.outerRadius);
                break;
            case mango::StaticMesh::MeshType::Sphere:
                genSphere(m_primitiveProperties.radius, m_primitiveProperties.slices);
                break;
            case mango::StaticMesh::MeshType::Torus:
                genTorus(m_primitiveProperties.innerRadius, m_primitiveProperties.outerRadius, m_primitiveProperties.slices, m_primitiveProperties.stacks);
                break;
            case mango::StaticMesh::MeshType::TrefoilKnot:
                genTrefoilKnot(m_primitiveProperties.slices, m_primitiveProperties.stacks);
                break;
            case mango::StaticMesh::MeshType::Quad:
                genQuad(m_primitiveProperties.width, m_primitiveProperties.height);
                break;
        }
    }

    void StaticMesh::setMeshType(MeshType type)
    {
        m_modelType = type; 
        
        if (MeshType::StaticMesh == m_modelType || MeshType::AnimatedMesh == m_modelType)
        {
            load(m_filename);
        }
        else
        {
            regeneratePrimitive();
        }
    }
}
