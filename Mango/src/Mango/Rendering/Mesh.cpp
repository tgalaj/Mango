#include "mgpch.h"

#include "Mesh.h"
#include "Mango/Core/AssetManager.h"
#include "Mango/Core/Services.h"

namespace mango
{

    void Mesh::bind() const
    {
        glBindVertexArray(m_vaoName);
    }

    void Mesh::render(uint32_t submeshIndex, uint32_t instancesCount)
    {
        MG_PROFILE_ZONE_SCOPED;

        if (instancesCount == 0)
        {
            glDrawElementsBaseVertex(GLenum(m_drawMode),
                                     m_submeshes[submeshIndex].indicesCount,
                                     GL_UNSIGNED_INT,
                                     (void*)(sizeof(uint32_t) * m_submeshes[submeshIndex].baseIndex),
                                     m_submeshes[submeshIndex].baseVertex);
        }
        else
        {
            glDrawElementsInstancedBaseVertex(GLenum(m_drawMode),
                                              m_submeshes[submeshIndex].indicesCount,
                                              GL_UNSIGNED_INT,
                                              (void*)(sizeof(uint32_t) * m_submeshes[submeshIndex].baseIndex),
                                              instancesCount,
                                              m_submeshes[submeshIndex].baseVertex);
        }
    }

    void Mesh::createBuffers(VertexData& vertexData)
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
    void Mesh::addAttributeBuffer(GLuint attribIndex, GLuint bindingIndex, GLint formatSize, GLenum dataType, GLuint bufferID, GLsizei stride, GLuint divisor)
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

    void Mesh::calcTangentSpace(VertexData& vertexData)
    {
        MG_PROFILE_ZONE_SCOPED;

        vertexData.tangents.resize(vertexData.positions.size());
        std::fill(std::begin(vertexData.tangents), std::end(vertexData.tangents), glm::vec3(0.0f));

        for (uint32_t i = 0; i < vertexData.indices.size(); i += 3)
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

    void Mesh::genPrimitive(VertexData& vertexData, bool generateTangents /*= true*/)
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
        submesh.baseIndex     = 0;
        submesh.baseVertex    = 0;
        submesh.indicesCount  = vertexData.indices.size();
        submesh.materialIndex = 0;

        if(!m_materialTable.empty()) m_materialTable.clear();
        
        m_materialTable.emplace_back(AssetManager::getMaterial("DefaultMaterial"));

        m_submeshes.emplace_back(submesh);
    }

    void Mesh::genCone(float height, float radius, uint32_t slices, uint32_t stacks)
    {
        MG_PROFILE_ZONE_SCOPED;
        VertexData vertexData;

        float thetaInc   = glm::two_pi<float>() / float(slices);
        float theta      = 0.0f;
        float halfHeight = height * 0.5f;

        /* Center bottom */

        vertexData.positions.push_back(glm::vec3(0.0f, -halfHeight, -0.0f));
        vertexData.normals  .push_back(glm::vec3(0.0f, -1.0f, 0.0f));
        vertexData.texcoords.push_back(glm::vec2(0.5f, 0.5f));

        /* Bottom */
        for (uint32_t sideCount = 0; sideCount <= slices; ++sideCount, theta += thetaInc)
        {
            vertexData.positions.push_back(glm::vec3(glm::cos(theta) * radius, -halfHeight, -glm::sin(theta) * radius));
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
                                                -halfHeight + height * level,
                                                -glm::sin(theta) * radius * (1.0f - level)));
                vertexData.normals  .push_back( glm::vec3(glm::cos(theta) * height / l, radius / l, -glm::sin(theta) * height / l));
                vertexData.texcoords.push_back( glm::vec2(sliceCount / float(slices), 1.0f - level));
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

    void Mesh::genCube(float size, float texcoordScale)
    {
        MG_PROFILE_ZONE_SCOPED;
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

    void Mesh::genCubeMap(float radius)
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

    void Mesh::genCylinder(float height, float radius, uint32_t slices)
    {
        MG_PROFILE_ZONE_SCOPED;
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

    void Mesh::genPlane(uint32_t resolution)
    {
        MG_PROFILE_ZONE_SCOPED;
        VertexData vertexData;

        float     halfResolution = resolution * 0.5f;
        glm::vec3 v              = glm::vec3(-halfResolution, 0.0, -halfResolution);

        for (uint32_t i = 0; i <= resolution; ++i)
        {
            for (uint32_t j = 0; j <= resolution; ++j)
            {
                vertexData.positions.push_back(v);
                vertexData.normals  .push_back(glm::vec3(0.0f, 1.0f, 0.0f));
                vertexData.texcoords.push_back(glm::vec2(i, j));

                v.z += 1.0;
            }
            v.z  = -halfResolution;
            v.x += 1.0f;
        }

        uint32_t idx = 0;
        for (uint32_t i = 0; i < resolution; ++i)
        {
            for (uint32_t j = 0; j < resolution; ++j)
            {
                vertexData.indices.push_back(idx);
                vertexData.indices.push_back(idx + 1);
                vertexData.indices.push_back(idx + resolution + 1);

                vertexData.indices.push_back(idx + 1);
                vertexData.indices.push_back(idx + resolution + 2);
                vertexData.indices.push_back(idx + resolution + 1);

                ++idx;
            }

            ++idx;
        }

        genPrimitive(vertexData);
    }

    void Mesh::genSphere(float radius, uint32_t slices)
    {
        MG_PROFILE_ZONE_SCOPED;
        VertexData vertexData;

        float deltaPhi = glm::two_pi<float>() / static_cast<float>(slices);

        uint32_t parallels = static_cast<uint32_t>(slices * 0.5f);

        for (uint32_t i = 0; i <= parallels; ++i)
        {
            for (uint32_t j = 0; j <= slices; ++j)
            {
                glm::vec3 p = glm::vec3(radius * glm::sin(deltaPhi * i) * glm::sin(deltaPhi * j),
                                        radius * glm::cos(deltaPhi * i),
                                        radius * glm::sin(deltaPhi * i) * glm::cos(deltaPhi * j));
                vertexData.positions.emplace_back(p);
                vertexData.normals  .emplace_back(glm::normalize(p));
                vertexData.texcoords.emplace_back(glm::vec2(j / static_cast<float>(slices),
                                                         i / static_cast<float>(parallels)));
            }
        }

        for (uint32_t i = 0; i < parallels; ++i)
        {
            for (uint32_t j = 0; j < slices; ++j)
            {
                vertexData.indices.emplace_back(i       * (slices + 1) + j);
                vertexData.indices.emplace_back((i + 1) * (slices + 1) + j);
                vertexData.indices.emplace_back((i + 1) * (slices + 1) + (j + 1));

                vertexData.indices.emplace_back(i       * (slices + 1) + j);
                vertexData.indices.emplace_back((i + 1) * (slices + 1) + (j + 1));
                vertexData.indices.emplace_back(i       * (slices + 1) + (j + 1));
            }
        }

        genPrimitive(vertexData);
    }

    void Mesh::genSphere(uint32_t divisions /*= 3*/)
    {
        MG_PROFILE_ZONE_SCOPED;

        if (divisions > 6)
        {
            divisions = 6;
            MG_CORE_WARN("Octasphere subdivisions decreased to maximum, which is 6.");
        }

        VertexData vertexData;

        // Prepare helper lambdas
        auto createLowerStrip = [&vertexData](uint32_t steps, uint32_t vTop, uint32_t vBottom, uint32_t t)
        {
            for (uint32_t i = 1; i < steps; ++i)
            {
                vertexData.indices[t++] = vBottom;
                vertexData.indices[t++] = vTop - 1;
                vertexData.indices[t++] = vTop;

                vertexData.indices[t++] = vBottom++;
                vertexData.indices[t++] = vTop++;
                vertexData.indices[t++] = vBottom;
            }

            vertexData.indices[t++] = vBottom;
            vertexData.indices[t++] = vTop - 1;
            vertexData.indices[t++] = vTop;

            return t;
        };

        auto createUpperStrip = [&vertexData](uint32_t steps, uint32_t vTop, uint32_t vBottom, uint32_t t)
        {
            vertexData.indices[t++] = vBottom;
            vertexData.indices[t++] = vTop - 1;
            vertexData.indices[t++] = ++vBottom;

            for (uint32_t i = 1; i <= steps; ++i)
            {
                vertexData.indices[t++] = vTop - 1;
                vertexData.indices[t++] = vTop;
                vertexData.indices[t++] = vBottom;

                vertexData.indices[t++] = vBottom;
                vertexData.indices[t++] = vTop++;
                vertexData.indices[t++] = ++vBottom;
            }

            return t;
        };

        auto createVertexLine = [&vertexData](const glm::vec3& start, const glm::vec3& end, uint32_t steps, uint32_t v) -> uint32_t
        {
            for (uint32_t i = 1; i <= steps; ++i)
            {
                vertexData.positions[v++] = glm::mix(start, end, (float)i / steps);
            }

            return v;
        };

        auto normalizeData = [&vertexData]()
        {
            for (uint32_t i = 0; i < vertexData.positions.size(); ++i)
            {
                vertexData.positions[i] = glm::normalize(vertexData.positions[i]);
                vertexData.normals  [i] = vertexData.positions[i];
            }
        };

        auto calcUVs = [&vertexData]()
        {
            float previousX = 1.0f;
            for (uint32_t i = 0; i < vertexData.positions.size(); ++i)
            {
                auto v = vertexData.positions[i];

                if (v.x == previousX)
                {
                    vertexData.texcoords[i - 1].x = 1.0f;
                }
                previousX = v.x;

                glm::vec2 uv;
                float p = glm::atan2(v.x, v.z);
                      p = (p < 0.0f) ? (p + glm::two_pi<float>()) : p;

                uv.x = p / -glm::two_pi<float>();
                uv.y = glm::asin(glm::clamp(v.y, -1.0f, 1.0f)) / -glm::pi<float>() + 0.5f;

                if (uv.x < 0.0f) uv.x += 1.0f;

                vertexData.texcoords[i] = uv;
            }

            // manually fix the twisted texture coordinates
            vertexData.texcoords[vertexData.positions.size() - 4].x = vertexData.texcoords[0].x = 0.125f;
            vertexData.texcoords[vertexData.positions.size() - 3].x = vertexData.texcoords[1].x = 0.375f;
            vertexData.texcoords[vertexData.positions.size() - 2].x = vertexData.texcoords[2].x = 0.625f;
            vertexData.texcoords[vertexData.positions.size() - 1].x = vertexData.texcoords[3].x = 0.875f;

            for (auto& uv : vertexData.texcoords)
            {
                uv.x = 1.0f - uv.x;
            }
        };

        auto calcTangents = [&vertexData]()
        {
            const uint32_t count = vertexData.positions.size();

            for (uint32_t i = 0; i < count; ++i)
            {
                glm::vec3 v = vertexData.positions[i];
                v.y = 0.0f;
                v = glm::normalize(v);

                glm::vec3 tangent;
                tangent.x = v.z;
                tangent.y = 0.0f;
                tangent.z = -v.x;

                vertexData.tangents[i] = tangent;
            }

            vertexData.tangents[count - 4] = vertexData.tangents[0] = glm::normalize(glm::vec3(-1.0f, 0.0f, -1.0f));
            vertexData.tangents[count - 3] = vertexData.tangents[1] = glm::normalize(glm::vec3( 1.0f, 0.0f, -1.0f));
            vertexData.tangents[count - 2] = vertexData.tangents[2] = glm::normalize(glm::vec3( 1.0f, 0.0f,  1.0f));
            vertexData.tangents[count - 1] = vertexData.tangents[3] = glm::normalize(glm::vec3(-1.0f, 0.0f,  1.0f));
        };

        // Resize the buffers
        const uint32_t resolution    = 1 << divisions;
        const uint32_t verticesCount = (resolution + 1) * (resolution + 1) * 4 - (resolution * 2 - 1) * 3;
        const uint32_t indicesCount  = (1 << (divisions * 2 + 3)) * 3;

        vertexData.positions.resize(verticesCount);
        vertexData.texcoords.resize(verticesCount);
        vertexData.normals.resize(verticesCount);
        vertexData.tangents.resize(verticesCount);
        vertexData.indices.resize(indicesCount);

        // Calculate vertex positions
        uint32_t v = 0, vBottom = 0, t = 0;

        for (uint32_t i = 0; i < 4; ++i)
        {
            vertexData.positions[v++] = math::down;
        }

        glm::vec3 directions[] = 
        {
            math::left,
            math::forward,
            math::right,
            math::back
        };

        // bottom hemisphere
        for (uint32_t i = 1; i <= resolution; ++i)
        {
            float deltaRes = (float)i / resolution;
            glm::vec3 start, end;

            vertexData.positions[v++] = end = glm::mix(math::down, math::back, deltaRes);
            
            for (uint32_t d = 0; d < std::size(directions); ++d)
            {
                start   = end;
                end     = glm::mix(math::down, directions[d], deltaRes);
                t       = createLowerStrip(i, v, vBottom, t);
                v       = createVertexLine(start, end, i, v);
                vBottom += (i > 1) ? (i - 1) : 1;
            }
            vBottom = v - 1 - i * 4;
        }

        // top hemisphere
        for (uint32_t i = resolution - 1; i >= 1; --i)
        {
            float deltaRes = (float)i / resolution;
            glm::vec3 start, end;

            vertexData.positions[v++] = end = glm::mix(math::up, math::back, deltaRes);

            for (uint32_t d = 0; d < std::size(directions); ++d)
            {
                start    = end;
                end      = glm::mix(math::up, directions[d], deltaRes);
                t        = createUpperStrip(i, v, vBottom, t);
                v        = createVertexLine(start, end, i, v);
                vBottom += i + 1;
            }
            vBottom = v - 1 - i * 4;
        }

        // top row
        for (uint32_t i = 0; i < 4; ++i)
        {
            vertexData.indices[t++] = vBottom;
            vertexData.indices[t++] = v;
            vertexData.indices[t++] = ++vBottom;

            vertexData.positions[v++] = math::up;
        }

        normalizeData();
        calcUVs();
        calcTangents();

        for (uint32_t i = 0; i < vertexData.positions.size(); ++i)
        {
            vertexData.positions[i] *= 0.5f;
        }

        genPrimitive(vertexData, false);
    }

    void Mesh::genTorus(float innerRadius, float outerRadius, uint32_t slices, uint32_t stacks)
    {
        MG_PROFILE_ZONE_SCOPED;
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

    /* Implementation inspired by: https://blackpawn.com/texts/pqtorus/default.html */
    void Mesh::genPQTorusKnot(uint32_t slices, uint32_t stacks, int p, int q, float knotR, float tubeR)
    {
        MG_PROFILE_ZONE_SCOPED;
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
                 r          = knotR * (0.5 * (2.0 + glm::sin(q * theta_next)));
            auto pNext      = glm::vec3(glm::cos(p * theta_next), glm::cos(q * theta_next), glm::sin(p * theta_next)) * r;

            auto T = pNext - P;
            auto N = pNext + P;
            auto B = glm::normalize(glm::cross(T, N));
                 N = glm::normalize(glm::cross(B, T)); /* corrected normal */

            for (uint32_t stack = 0; stack <= stacks; ++stack, phi += phiStep)
            {
                glm::vec2 circleVertexPosition = glm::vec2(glm::cos(phi), glm::sin(phi)) * tubeR;

                vertexData.positions.push_back(N * circleVertexPosition.x + B * circleVertexPosition.y + P);
                vertexData.normals  .push_back(glm::normalize(vertexData.positions.back() - P));
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

    void Mesh::genQuad()
    {
        MG_PROFILE_ZONE_SCOPED;
        m_drawMode = DrawMode::TRIANGLES;

        VertexData vertexData;

        float halfWidth  = 0.5f;
        float halfHeight = 0.5f;

        vertexData.positions = {
            glm::vec3(-halfWidth, -halfHeight, 0.0f),
            glm::vec3(-halfWidth,  halfHeight, 0.0f),
            glm::vec3(halfWidth,  -halfHeight, 0.0f),
            glm::vec3(halfWidth,   halfHeight, 0.0f)
        };

        vertexData.texcoords = {
            glm::vec2(0.0f, 1.0f),
            glm::vec2(0.0f, 0.0f),
            glm::vec2(1.0f, 1.0f),
            glm::vec2(1.0f, 0.0f),
        };

        vertexData.normals = {
            glm::vec3(0.0f, 0.0f, 1.0f),
            glm::vec3(0.0f, 0.0f, 1.0f),
            glm::vec3(0.0f, 0.0f, 1.0f),
            glm::vec3(0.0f, 0.0f, 1.0f),
        };

        vertexData.tangents = {
            glm::vec3(1.0f, 0.0f, 0.0f),
            glm::vec3(1.0f, 0.0f, 0.0f),
            glm::vec3(1.0f, 0.0f, 0.0f),
            glm::vec3(1.0f, 0.0f, 0.0f),
        };

        vertexData.indices.push_back(0);
        vertexData.indices.push_back(3);
        vertexData.indices.push_back(1);
        vertexData.indices.push_back(0);
        vertexData.indices.push_back(2);
        vertexData.indices.push_back(3);

        genPrimitive(vertexData, false);
    }
}
