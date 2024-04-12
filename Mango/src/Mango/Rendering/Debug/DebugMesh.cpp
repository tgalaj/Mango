#include "DebugMesh.h"

namespace mango
{

    ref<Mesh> DebugMesh::createDebugBox()
    {
        if (s_debugBox)
        {
            return s_debugBox;
        }

        s_debugBox = createRef<Mesh>();
        
        VertexData data;
        data.positions.resize(8);
        data.indices.resize(12);

        data.positions =
        {
            glm::vec3(-1.0f, -1.0f, +1.0f),
            glm::vec3(+1.0f, -1.0f, +1.0f),
            glm::vec3(+1.0f, +1.0f, +1.0f),
            glm::vec3(-1.0f, +1.0f, +1.0f),

            glm::vec3(-1.0f, -1.0f, -1.0f),
            glm::vec3(+1.0f, -1.0f, -1.0f),
            glm::vec3(+1.0f, +1.0f, -1.0f),
            glm::vec3(-1.0f, +1.0f, -1.0f)
        };

        data.indices =
        {
            0, 1,
            1, 2,
            2, 3,
            3, 0,

            4, 5,
            5, 6,
            6, 7,
            7, 4,

            0, 4,
            1, 5,
            2, 6,
            3, 7
        };

        s_debugBox->build(data, Mesh::DrawMode::LINES);

        return s_debugBox;
    }

    ref<Mesh> DebugMesh::createDebugCapsule()
    {
        if (s_debugCapsule)
        {
            return s_debugCapsule;
        }

        s_debugCapsule = createRef<Mesh>();
        VertexData capsuleColliderData;

        float radius             = 0.5f;
        float depth              = 1.0f;
        float halfCylinderHeight = depth / 2.0f;

        // two XZ circles at the upper and lower half height of the capsule's cylinder
        uint32_t samples    = 32;
        float    deltaTheta = glm::two_pi<float>() / samples;

        for (uint32_t s = 0; s < samples; ++s)
        {
            float x = radius * glm::cos(s * deltaTheta);
            float z = radius * glm::sin(s * deltaTheta);

            capsuleColliderData.positions.emplace_back(glm::vec3(x, halfCylinderHeight, z));
        }

        for (uint32_t s = 0; s < samples; ++s)
        {
            float x = radius * glm::cos(s * deltaTheta);
            float z = radius * glm::sin(s * deltaTheta);

            capsuleColliderData.positions.emplace_back(glm::vec3(x, -halfCylinderHeight, z));
        }

        // indices of the XZ half circles
        uint32_t currentBeginIndex = 0;
        uint32_t currentEndIndex   = samples;

        for (uint32_t c = 0; c < 2; ++c)
        {
            for (uint32_t i = currentBeginIndex; i < currentEndIndex; ++i)
            {
                capsuleColliderData.indices.emplace_back(i);

                if (i < currentEndIndex - 1) capsuleColliderData.indices.emplace_back((i + 1));
                else                         capsuleColliderData.indices.emplace_back(currentBeginIndex);
            }

            currentBeginIndex += samples;
            currentEndIndex   += samples;
        }

        // XY plane capsule 2D
        // two half circles
        deltaTheta = glm::pi<float>() / samples;
        for (uint32_t s = 0; s < samples; ++s)
        {
            float x = radius * glm::cos(s * deltaTheta);
            float y = radius * glm::sin(s * deltaTheta);

            capsuleColliderData.positions.emplace_back(glm::vec3(x, y + halfCylinderHeight, 0.0f));
        }

        for (uint32_t s = 0; s < samples; ++s)
        {
            float x = radius * glm::cos(s * deltaTheta);
            float y = radius * glm::sin(s * deltaTheta);

            capsuleColliderData.positions.emplace_back(glm::vec3(x, -y - halfCylinderHeight, 0.0f));
        }

        // YZ plane capsule 2D
        // two half circles
        for (uint32_t s = 0; s < samples; ++s)
        {
            float z = radius * glm::cos(s * deltaTheta);
            float y = radius * glm::sin(s * deltaTheta);

            capsuleColliderData.positions.emplace_back(glm::vec3(0.0f, y + halfCylinderHeight, z));
        }

        for (uint32_t s = 0; s < samples; ++s)
        {
            float z = radius * glm::cos(s * deltaTheta);
            float y = radius * glm::sin(s * deltaTheta);

            capsuleColliderData.positions.emplace_back(glm::vec3(0.0f, -y - halfCylinderHeight, z));
        }

        // indices for both 2D capsules
        for (uint32_t c = 0; c < 2; ++c)
        {
            uint32_t beginLineIndex1 = currentBeginIndex;
            uint32_t endLineIndex1   = beginLineIndex1 + samples;

            uint32_t beginLineIndex2 = endLineIndex1 - 1;
            uint32_t endLineIndex2   = beginLineIndex2 + samples;

            for (uint32_t hc = 0; hc < 2; ++hc)
            {
                for (uint32_t i = currentBeginIndex; i < currentEndIndex - 1; ++i)
                {
                    capsuleColliderData.indices.emplace_back(i);
                    capsuleColliderData.indices.emplace_back((i + 1));
                }

                currentBeginIndex += samples;
                currentEndIndex   += samples;
            }

            // Create two lines connecting the upper and lower half-circles
            capsuleColliderData.indices.emplace_back(beginLineIndex1);
            capsuleColliderData.indices.emplace_back(endLineIndex1);

            capsuleColliderData.indices.emplace_back(beginLineIndex2);
            capsuleColliderData.indices.emplace_back(endLineIndex2);
        }

        // Build the mesh
        s_debugCapsule->build(capsuleColliderData, Mesh::DrawMode::LINES);

        return s_debugCapsule;
    }

    ref<Mesh> DebugMesh::createDebugSphere()
    {
        if (s_debugSphere)
        {
            return s_debugSphere;
        }

        s_debugSphere = createRef<Mesh>();

        VertexData sphereColliderData;
        
        uint32_t samples    = 32;
        float    deltaTheta = glm::two_pi<float>() / samples;
        float    radius     = 1.0f;
        
        // XY plane
        for (uint32_t s = 0; s < samples; ++s)
        {
            float x = radius * glm::cos(s * deltaTheta);
            float y = radius * glm::sin(s * deltaTheta);

            sphereColliderData.positions.emplace_back(glm::vec3(x, y, 0.0f));
        }
        // XZ plane
        for (uint32_t s = 0; s < samples; ++s)
        {
            float x = radius * glm::cos(s * deltaTheta);
            float z = radius * glm::sin(s * deltaTheta);

            sphereColliderData.positions.emplace_back(glm::vec3(x, 0.0f, z));
        }

        // YZ plane
        for (uint32_t s = 0; s < samples; ++s)
        {
            float y = radius * glm::cos(s * deltaTheta);
            float z = radius * glm::sin(s * deltaTheta);

            sphereColliderData.positions.emplace_back(glm::vec3(0.0f, y, z));
        }

        // indices
        uint32_t currentBeginIndex = 0;
        uint32_t currentEndIndex   = samples;

        for (uint32_t circle = 0; circle < 3; ++circle)
        {
            for (uint32_t i = currentBeginIndex; i < currentEndIndex; ++i)
            {
                sphereColliderData.indices.emplace_back(i);

                if (i < currentEndIndex - 1) sphereColliderData.indices.emplace_back((i + 1));
                else                         sphereColliderData.indices.emplace_back(currentBeginIndex);
            }

            currentBeginIndex += samples;
            currentEndIndex   += samples;
        }

        s_debugSphere->build(sphereColliderData, Mesh::DrawMode::LINES);

        return s_debugSphere;
    }

    ref<Mesh> DebugMesh::createDebugCone()
    {
        MG_CORE_ASSERT_FAIL("DebugMesh::createDebugCone() is not yet implemented!");
        return s_debugCone;
    }

}