#if 0
#pragma once

#include "Mesh.h"

#include <glm/mat2x4.hpp>
#include <glm/mat4x4.hpp>
#include <unordered_map>

namespace mango
{
    class AnimatedMesh : public Mesh
    {
    public:
        AnimatedMesh() : m_bonesCount            (0), 
                         m_globalInverseTransform(glm::mat4(1.0)), 
                         m_assimpScene           (nullptr), 
                         m_animationSpeed        (1.0),
                         m_currentAnimationTime  (0.0), 
                         m_currentAnimationIndex (0), 
                         m_animationsCount       (0) {}

        virtual ~AnimatedMesh() {}

        /* Used for Linear Blend Skinning */
        void boneTransform(float dt, std::vector<glm::mat4>& transforms);

        /* Used for Dual Quaternion Blend Skinning */
        void boneTransform(float dt, std::vector<glm::mat2x4>& transforms);

        std::vector<std::string> getAnimationsNames() const;
        uint32_t                 getAnimationsCount() const { return m_animationsCount; }
        uint32_t                 getBonesCount()      const { return m_bonesCount; }

        void setAnimation(uint32_t animation_index) 
        { 
            m_currentAnimationIndex = std::max(0u, std::min(animation_index, m_animationsCount - 1)); 
            m_currentAnimationTime  = 0.0f; 
        }

        void setAnimationSpeed(float speed)
        {
            m_animationSpeed = std::max(speed, 0.0f);
        }

    protected:
        static const uint32_t NUM_BONES_PER_VERTEX = 4;

        struct BoneInfo
        {
            glm::mat4 m_boneOffset;
            glm::mat4 m_finalTransform;

            BoneInfo()
            {
                m_boneOffset     = glm::mat4(0.0);
                m_finalTransform = glm::mat4(0.0);
            }
        };

        struct VertexBoneData
        {
            int   m_ids[NUM_BONES_PER_VERTEX];
            float m_weights[NUM_BONES_PER_VERTEX];
            
            VertexBoneData()
            {
                reset();
            }

            void reset()
            {
                memset(m_ids,     0, sizeof(m_ids));
                memset(m_weights, 0, sizeof(m_weights));
            }

            void addBoneData(int boneID, float weight)
            {
                for (uint32_t i = 0; i < NUM_BONES_PER_VERTEX; ++i)
                {
                    if (m_weights[i] == 0.0)
                    {
                        m_ids    [i] = boneID;
                        m_weights[i] = weight;
                        return;
                    }
                }

                // should never get here - more bones that we have space for
                assert(0);
            }
        };

        glm::mat4 operator=(const aiMatrix4x4& from)
        {
            glm::mat4 to;

            // the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
            to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
            to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
            to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
            to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;

            return to;
        }

        virtual void calcInterpolatedScaling (aiVector3D&   out, float animationTime, const aiNodeAnim* nodeAnim);
        virtual void calcInterpolatedRotation(aiQuaternion& out, float animationTime, const aiNodeAnim* nodeAnim);
        virtual void calcInterpolatedPosition(aiVector3D&   out, float animationTime, const aiNodeAnim* nodeAnim);

        virtual uint32_t findScaling (float animationTime, const aiNodeAnim* nodeAnim);
        virtual uint32_t findRotation(float animationTime, const aiNodeAnim* nodeAnim);
        virtual uint32_t findPosition(float animationTime, const aiNodeAnim* nodeAnim);

        virtual const aiNodeAnim* findNodeAnim(const aiAnimation* animation, std::string_view nodeName);
        virtual void readNodeHierarchy(float animationTime, const aiNode* node, const glm::mat4& parentTransform);

        bool load(const std::string& filename) override;
        bool parseScene(const aiScene* scene, const std::filesystem::path& parentDirectory) override;
        virtual void loadBones (uint32_t submeshIndex, const aiMesh* mesh, std::vector<VertexBoneData>& bones);

        virtual void createBuffers(VertexData& vertexData, std::vector<VertexBoneData>& bonesData);
        
        std::unordered_map<std::string, uint32_t> m_bonesMapping;
        std::vector<BoneInfo>                     m_boneInfos;

        uint32_t  m_bonesCount;
        glm::mat4 m_globalInverseTransform;

        const aiScene*   m_assimpScene;
        Assimp::Importer m_importer;

        float    m_animationSpeed;
        float    m_currentAnimationTime;
        uint32_t m_currentAnimationIndex;
        uint32_t m_animationsCount;
    };
}
#endif