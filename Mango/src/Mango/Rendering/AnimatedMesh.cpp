#include "mgpch.h"

#include "AnimatedMesh.h"

#include <glm/gtc/matrix_transform.hpp>
#include <assimp/postprocess.h>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/dual_quaternion.hpp>

namespace mango
{
    void AnimatedMesh::boneTransform(float dt, std::vector<glm::mat4>& transforms)
    {
        MG_PROFILE_ZONE_SCOPED;
        if (m_assimpScene->mNumAnimations > 0)
        {
            /* Calc animation duration */
            auto ticksPerSecond    = (float)(m_assimpScene->mAnimations[m_currentAnimationIndex]->mTicksPerSecond != 0 ? m_assimpScene->mAnimations[m_currentAnimationIndex]->mTicksPerSecond : 25.0f);
            auto animationDuration = (float)m_assimpScene->mAnimations[m_currentAnimationIndex]->mDuration;

            m_currentAnimationTime += ticksPerSecond * dt * m_animationSpeed;
            m_currentAnimationTime = fmod(m_currentAnimationTime, animationDuration);

            readNodeHierarchy(m_currentAnimationTime, m_assimpScene->mRootNode, glm::mat4(1.0f));

            transforms.resize(m_bonesCount);

            for (uint32_t i = 0; i < m_bonesCount; ++i)
            {
                transforms[i] = m_boneInfos[i].m_finalTransform;
            }
        }
    }

    void AnimatedMesh::boneTransform(float dt, std::vector<glm::mat2x4>& transforms)
    {
        MG_PROFILE_ZONE_SCOPED;
        if (m_assimpScene->mNumAnimations > 0)
        {
            /* Calc animation duration */
            auto ticksPerSecond    = (float)(m_assimpScene->mAnimations[m_currentAnimationIndex]->mTicksPerSecond != 0 ? m_assimpScene->mAnimations[m_currentAnimationIndex]->mTicksPerSecond : 25.0f);
            auto animationDuration = (float)m_assimpScene->mAnimations[m_currentAnimationIndex]->mDuration;

            m_currentAnimationTime += ticksPerSecond * dt * m_animationSpeed;
            m_currentAnimationTime = fmod(m_currentAnimationTime, animationDuration);

            readNodeHierarchy(m_currentAnimationTime, m_assimpScene->mRootNode, glm::mat4(1.0f));

            transforms.resize(m_bonesCount);

            for (uint32_t i = 0; i < m_bonesCount; ++i)
            {
                 glm::quat rotation(m_boneInfos[i].m_finalTransform);
                 glm::fdualquat dq (rotation, m_boneInfos[i].m_finalTransform[3]);

                 glm::mat2x4 dqMat(glm::vec4(dq.real.w, dq.real.x, dq.real.y, dq.real.z), 
                                   glm::vec4(dq.dual.w, dq.dual.x, dq.dual.y, dq.dual.z));

                 transforms[i] = dqMat;
            }
        }
    }

    void AnimatedMesh::calcInterpolatedScaling(aiVector3D& out, float animationTime, const aiNodeAnim* nodeAnim)
    {
        MG_PROFILE_ZONE_SCOPED;
        if (nodeAnim->mNumScalingKeys == 1)
        {
            out = nodeAnim->mScalingKeys[0].mValue;
            return;
        }

        uint32_t scalingIndex     = findScaling(animationTime, nodeAnim);
        uint32_t nextScalingIndex = (scalingIndex + 1);

        MG_CORE_ASSERT(nextScalingIndex < nodeAnim->mNumScalingKeys);

        auto  deltaTime = (float)(nodeAnim->mScalingKeys[nextScalingIndex].mTime - nodeAnim->mScalingKeys[scalingIndex].mTime);
        float factor    = (animationTime - (float)nodeAnim->mScalingKeys[scalingIndex].mTime) / deltaTime;

        MG_CORE_ASSERT(factor >= 0.0f && factor <= 1.0f);

        const aiVector3D& start = nodeAnim->mScalingKeys[scalingIndex].mValue;
        const aiVector3D& end   = nodeAnim->mScalingKeys[nextScalingIndex].mValue;

        aiVector3D delta = end - start;
        out = start + factor * delta;
    }

    void AnimatedMesh::calcInterpolatedRotation(aiQuaternion& out, float animationTime, const aiNodeAnim* nodeAnim)
    {
        MG_PROFILE_ZONE_SCOPED;
        // we need at least two values to interpolate...
        if (nodeAnim->mNumRotationKeys == 1)
        {
            out = nodeAnim->mRotationKeys[0].mValue;
            return;
        }

        uint32_t rotationIndex     = findRotation(animationTime, nodeAnim);
        uint32_t nextRotationIndex = (rotationIndex + 1);

        MG_CORE_ASSERT(nextRotationIndex < nodeAnim->mNumRotationKeys);

        auto  deltaTime = (float)(nodeAnim->mRotationKeys[nextRotationIndex].mTime - nodeAnim->mRotationKeys[rotationIndex].mTime);
        float factor    = (animationTime - (float)nodeAnim->mRotationKeys[rotationIndex].mTime) / deltaTime;

        MG_CORE_ASSERT(factor >= 0.0f && factor <= 1.0f);

        const aiQuaternion& startRotationQuat = nodeAnim->mRotationKeys[rotationIndex].mValue;
        const aiQuaternion& endRotationQuat   = nodeAnim->mRotationKeys[nextRotationIndex].mValue;

        aiQuaternion::Interpolate(out, startRotationQuat, endRotationQuat, factor);
        out = out.Normalize();

    }

    void AnimatedMesh::calcInterpolatedPosition(aiVector3D& out, float animationTime, const aiNodeAnim* nodeAnim)
    {
        MG_PROFILE_ZONE_SCOPED;
        if (nodeAnim->mNumPositionKeys == 1)
        {
            out = nodeAnim->mPositionKeys[0].mValue;
            return;
        }

        uint32_t positionIndex     = findPosition(animationTime, nodeAnim);
        uint32_t nextPositionIndex = (positionIndex + 1);

        MG_CORE_ASSERT(nextPositionIndex < nodeAnim->mNumPositionKeys);

        auto  deltaTime = (float)(nodeAnim->mPositionKeys[nextPositionIndex].mTime - nodeAnim->mPositionKeys[positionIndex].mTime);
        float factor    = (animationTime - (float)nodeAnim->mPositionKeys[positionIndex].mTime) / deltaTime;

        MG_CORE_ASSERT(factor >= 0.0f && factor <= 1.0f);

        const aiVector3D& start = nodeAnim->mPositionKeys[positionIndex].mValue;
        const aiVector3D& end  = nodeAnim->mPositionKeys[nextPositionIndex].mValue;

        aiVector3D delta = end - start;
        out = start + factor * delta;
    }

    uint32_t AnimatedMesh::findScaling(float animationTime, const aiNodeAnim* nodeAnim)
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_CORE_ASSERT(nodeAnim->mNumScalingKeys > 0);

        for (uint32_t i = 0; i < nodeAnim->mNumScalingKeys - 1; i++)
        {
            if (animationTime < (float)nodeAnim->mScalingKeys[i + 1].mTime)
            {
                return i;
            }
        }

        MG_CORE_ASSERT(0);

        return 0;
    }

    uint32_t AnimatedMesh::findRotation(float animationTime, const aiNodeAnim* nodeAnim)
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_CORE_ASSERT(nodeAnim->mNumRotationKeys > 0);

        for (uint32_t i = 0; i < nodeAnim->mNumRotationKeys - 1; i++)
        {
            if (animationTime < (float)nodeAnim->mRotationKeys[i + 1].mTime)
            {
                return i;
            }
        }

        MG_CORE_ASSERT(0);

        return 0;
    }

    uint32_t AnimatedMesh::findPosition(float animationTime, const aiNodeAnim* nodeAnim)
    {
        MG_PROFILE_ZONE_SCOPED;
        for (uint32_t i = 0; i < nodeAnim->mNumPositionKeys - 1; i++)
        {
            if (animationTime < (float)nodeAnim->mPositionKeys[i + 1].mTime)
            {
                return i;
            }
        }

        MG_CORE_ASSERT(0);

        return 0;
    }

    const aiNodeAnim* AnimatedMesh::findNodeAnim(const aiAnimation* animation, std::string_view nodeName)
    {
        MG_PROFILE_ZONE_SCOPED;
        for (uint32_t i = 0; i < animation->mNumChannels; i++)
        {
            const aiNodeAnim* node_anim = animation->mChannels[i];

            if (std::string(node_anim->mNodeName.data) == nodeName)
            {
                return node_anim;
            }
        }

        return nullptr;
    }

    void AnimatedMesh::readNodeHierarchy(float animationTime, const aiNode* node, const glm::mat4& parentTransform)
    {
        MG_PROFILE_ZONE_SCOPED;
              std::string  nodeName      = std::string(node->mName.data);
        const aiAnimation* animation     = m_assimpScene->mAnimations[m_currentAnimationIndex];
              glm::mat4    nodeTransform = mat4_cast(node->mTransformation);
        const aiNodeAnim*  nodeAnim      = findNodeAnim(animation, nodeName);

        if (nodeAnim)
        {
            // Interpolate scaling and generate scaling transformation matrix
            aiVector3D scaling;
            calcInterpolatedScaling(scaling, animationTime, nodeAnim);
            glm::mat4 scalingMat = glm::scale(glm::mat4(1.0), vec3_cast(scaling));

            // Interpolate rotation and generate rotation transformation matrix
            aiQuaternion rotationQuat;
            calcInterpolatedRotation(rotationQuat, animationTime, nodeAnim);
            glm::quat rotation    = quat_cast(rotationQuat);
            glm::mat4 rotationMat = glm::toMat4(rotation);

            // Interpolate translation and generate translation transformation matrix
            aiVector3D translation;
            calcInterpolatedPosition(translation, animationTime, nodeAnim);
            glm::mat4 translationMat = glm::translate(glm::mat4(1.0), vec3_cast(translation));

            // Combine the above transformations
            nodeTransform = translationMat * rotationMat * scalingMat;
        }

        glm::mat4 globalTransform = parentTransform * nodeTransform;

        if (m_bonesMapping.find(nodeName) != m_bonesMapping.end())
        {
            uint32_t boneIndex = m_bonesMapping[nodeName];
            m_boneInfos[boneIndex].m_finalTransform = m_globalInverseTransform * globalTransform * m_boneInfos[boneIndex].m_boneOffset;
        }

        for (uint32_t i = 0; i < node->mNumChildren; i++)
        {
            readNodeHierarchy(animationTime, node->mChildren[i], globalTransform);
        }
    }

    void AnimatedMesh::loadBones(uint32_t submeshIndex, const aiMesh* mesh, std::vector<VertexBoneData>& bones)
    {
        MG_PROFILE_ZONE_SCOPED;
        for (uint32_t i = 0; i < mesh->mNumBones; i++)
        {
            uint32_t boneIndex = 0;
            std::string boneName(mesh->mBones[i]->mName.data);

            if (m_bonesMapping.find(boneName) == m_bonesMapping.end())
            {
                // Allocate an index for a new bone
                boneIndex = m_bonesCount;
                m_bonesCount++;
                
                BoneInfo bi;
                m_boneInfos.push_back(bi);

                m_boneInfos[boneIndex].m_boneOffset = mat4_cast(mesh->mBones[i]->mOffsetMatrix);
                m_bonesMapping[boneName]             = boneIndex;
            }
            else
            {
                boneIndex = m_bonesMapping[boneName];
            }

            for (uint32_t j = 0; j < mesh->mBones[i]->mNumWeights; j++)
            {
                uint32_t vertexID = m_submeshes[submeshIndex].baseVertex + mesh->mBones[i]->mWeights[j].mVertexId;
                float    weight   = mesh->mBones[i]->mWeights[j].mWeight;

                bones[vertexID].addBoneData(boneIndex, weight);
            }
        }
    }

    bool AnimatedMesh::load(const std::string& filename)
    {
        MG_PROFILE_ZONE_SCOPED;
        /* Release the previously loaded mesh if it was loaded. */
        if (m_vaoName)
        {
            release();
        }

        m_modelType = MeshType::AnimatedMesh;
        m_filename  = filename;

        auto filepath = VFI::getFilepath(filename);

        /* Load model */
        m_assimpScene = m_importer.ReadFile(filepath.string(), aiProcess_Triangulate              | 
                                                               aiProcess_GenSmoothNormals         | 
                                                               aiProcess_GenUVCoords              |
                                                               aiProcess_CalcTangentSpace         |
                                                               aiProcess_FlipUVs                  |
                                                               aiProcess_JoinIdenticalVertices    | 
                                                               aiProcess_RemoveRedundantMaterials |
                                                               aiProcess_ImproveCacheLocality     |
                                                               aiProcess_GenBoundingBoxes );

        if (!m_assimpScene || m_assimpScene->mFlags == AI_SCENE_FLAGS_INCOMPLETE || !m_assimpScene->mRootNode)
        {
            MG_CORE_ERROR("Assimp error while loading mesh {}\n Error: {}", filepath.string(), m_importer.GetErrorString());
            return false;
        }

        m_globalInverseTransform = mat4_cast(m_assimpScene->mRootNode->mTransformation);
        m_globalInverseTransform = glm::inverse(m_globalInverseTransform);
        m_animationsCount        = m_assimpScene->mNumAnimations;

        auto parentDirectory = std::filesystem::path(filename).parent_path();

        return parseScene(m_assimpScene, parentDirectory);
    }

    std::vector<std::string> AnimatedMesh::getAnimationsNames() const
    {
        MG_PROFILE_ZONE_SCOPED;
        auto animationsCount = m_assimpScene->mNumAnimations;
        std::vector<std::string> animationsNames(animationsCount);

        for (uint32_t i = 0; i < animationsCount; ++i)
        {
            animationsNames[i] = m_assimpScene->mAnimations[i]->mName.C_Str();
        }

        return animationsNames;
    }

    bool AnimatedMesh::parseScene(const aiScene* scene, const std::filesystem::path& parentDirectory)
    {
        MG_PROFILE_ZONE_SCOPED;
        m_submeshes.resize(scene->mNumMeshes);

        for (uint32_t i = 0; i < m_submeshes.size(); ++i)
        {
            m_submeshes[i].material = std::make_shared<Material>();
        }

        VertexData vertexData;
        std::vector<VertexBoneData> bonesData;

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
        bonesData.resize(verticesCount);

        /* Load mesh parts one by one. */
        glm::vec3 min = glm::vec3(std::numeric_limits<float>::max());
        glm::vec3 max = -min;

        for (uint32_t i = 0; i < m_submeshes.size(); ++i)
        {
            aiMesh* mesh = scene->mMeshes[i];
            loadMeshPart(mesh, vertexData);
            loadBones(i, mesh, bonesData);

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
        createBuffers(vertexData, bonesData);

        return true;
    }

    void AnimatedMesh::createBuffers(VertexData& vertexData, std::vector<VertexBoneData>& bonesData)
    {
        MG_PROFILE_ZONE_SCOPED;
        bool hasTangents  = !vertexData.tangents.empty();
        bool hasBonesData = !bonesData.empty();

        std::vector<float> boneWeights; 
        boneWeights.reserve(bonesData.size() * NUM_BONES_PER_VERTEX);

        std::vector<int> boneIDs; 
        boneIDs.reserve(bonesData.size() * NUM_BONES_PER_VERTEX);

        if (hasBonesData)
        {
            for (uint32_t i = 0; i < bonesData.size(); ++i)
            {
                for (auto& boneWeight : bonesData[i].m_weights)
                {
                    boneWeights.push_back(boneWeight);
                }
            }

            for (uint32_t i = 0; i < bonesData.size(); ++i)
            {
                for (auto& boneID : bonesData[i].m_ids)
                {
                    boneIDs.push_back(boneID);
                }
            }
        }

        const GLsizei positionsSizeBytes = vertexData.positions.size() * sizeof(vertexData.positions[0]);
        const GLsizei texcoordsSizeBytes = vertexData.texcoords.size() * sizeof(vertexData.texcoords[0]);
        const GLsizei normalsSizeBytes   = vertexData.normals  .size() * sizeof(vertexData.normals  [0]);

        const GLsizei tangentsSizeBytes    = hasTangents   ? vertexData.tangents.size() * sizeof(vertexData.tangents[0]) : 0;
        const GLsizei boneWeightsSizeBytes = hasBonesData ? boneWeights         .size() * sizeof(boneWeights        [0]) : 0;
        const GLsizei boneIDsSizeBytes     = hasBonesData ? boneIDs             .size() * sizeof(boneIDs            [0]) : 0;

        const GLsizei totalSizeBytes = positionsSizeBytes + texcoordsSizeBytes + normalsSizeBytes + tangentsSizeBytes + boneWeightsSizeBytes + boneIDsSizeBytes;

        glCreateBuffers(1, &m_vboName);
        glNamedBufferStorage(m_vboName, totalSizeBytes, nullptr, GL_DYNAMIC_STORAGE_BIT);

        uint64_t offset = 0;
        glNamedBufferSubData(m_vboName, offset, positionsSizeBytes, vertexData.positions.data());
        
        offset += positionsSizeBytes;
        glNamedBufferSubData(m_vboName, offset, texcoordsSizeBytes, vertexData.texcoords.data());
        
        offset += texcoordsSizeBytes;
        glNamedBufferSubData(m_vboName, offset, normalsSizeBytes, vertexData.normals.data());
        
        if (hasTangents)
        {
            offset += normalsSizeBytes;
            glNamedBufferSubData(m_vboName, offset, tangentsSizeBytes, vertexData.tangents.data());
            offset += tangentsSizeBytes;
        }

        if (hasBonesData)
        {
            glNamedBufferSubData(m_vboName, offset, boneWeightsSizeBytes, boneWeights.data());
            offset += boneWeightsSizeBytes;

            glNamedBufferSubData(m_vboName, offset, boneIDsSizeBytes, boneIDs.data());
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
            offset += tangentsSizeBytes;
        }

        if (hasBonesData)
        {
            glVertexArrayVertexBuffer(m_vaoName, 4 /* bindingindex*/, m_vboName, offset, sizeof(boneWeights[0]) * 4 /*stride*/);
            offset += boneWeightsSizeBytes;

            glVertexArrayVertexBuffer(m_vaoName, 5 /* bindingindex*/, m_vboName, offset, sizeof(boneIDs[0]) * 4 /*stride*/);
        }

        glVertexArrayElementBuffer(m_vaoName, m_iboName);

        glEnableVertexArrayAttrib(m_vaoName, 0 /*attribindex*/); // positions
        glEnableVertexArrayAttrib(m_vaoName, 1 /*attribindex*/); // texcoords
        glEnableVertexArrayAttrib(m_vaoName, 2 /*attribindex*/); // normals

        if (hasTangents) 
        {
            glEnableVertexArrayAttrib(m_vaoName, 3 /*attribindex*/); // tangents
        }

        if (hasBonesData)
        {
            glEnableVertexArrayAttrib(m_vaoName, 4 /*attribindex*/); // bone weights
            glEnableVertexArrayAttrib(m_vaoName, 5 /*attribindex*/); // bone ids
        }

        glVertexArrayAttribFormat(m_vaoName, 0 /*attribindex */, 3 /* size */, GL_FLOAT, GL_FALSE, 0 /*relativeoffset*/); 
        glVertexArrayAttribFormat(m_vaoName, 1 /*attribindex */, 2 /* size */, GL_FLOAT, GL_FALSE, 0 /*relativeoffset*/); 
        glVertexArrayAttribFormat(m_vaoName, 2 /*attribindex */, 3 /* size */, GL_FLOAT, GL_FALSE, 0 /*relativeoffset*/); 

        if (hasTangents) 
        {
            glVertexArrayAttribFormat(m_vaoName, 3 /*attribindex */, 3 /* size */, GL_FLOAT, GL_FALSE, 0 /*relativeoffset*/);
        }

        if (hasBonesData)
        {
            glVertexArrayAttribFormat (m_vaoName, 4 /*attribindex */, 4 /* size */, GL_FLOAT, GL_FALSE, 0 /*relativeoffset*/);
            glVertexArrayAttribIFormat(m_vaoName, 5 /*attribindex */, 4 /* size */, GL_INT,             0 /*relativeoffset*/);
        }

        glVertexArrayAttribBinding(m_vaoName, 0 /*attribindex*/, 0 /*bindingindex*/); // positions
        glVertexArrayAttribBinding(m_vaoName, 1 /*attribindex*/, 1 /*bindingindex*/); // texcoords
        glVertexArrayAttribBinding(m_vaoName, 2 /*attribindex*/, 2 /*bindingindex*/); // normals

        if (hasTangents) 
        {
            glVertexArrayAttribBinding(m_vaoName, 3 /*attribindex*/, 3 /*bindingindex*/); // tangents
        }

        if (hasBonesData)
        {
            glVertexArrayAttribBinding(m_vaoName, 4 /*attribindex*/, 4 /*bindingindex*/); // bone weights
            glVertexArrayAttribBinding(m_vaoName, 5 /*attribindex*/, 5 /*bindingindex*/); // bone ids
        }
    }
}