#include "GeomPrimitive.h"
#include <glm\gtc\constants.hpp>

void GeomPrimitive::genCube(VEBuffers &buffers, float radius)
{
    float r2 = radius * 0.5f;

    buffers.positions = { 
                            glm::vec3(-r2, -r2,  r2), 
                            glm::vec3( r2, -r2,  r2),
                            glm::vec3( r2,  r2,  r2),
                            glm::vec3(-r2,  r2,  r2),     
                            
                            glm::vec3(-r2, -r2, -r2),
                            glm::vec3( r2, -r2, -r2),
                            glm::vec3( r2,  r2, -r2),
                            glm::vec3(-r2,  r2, -r2)
                        };

    buffers.normals   = { 
                            glm::normalize(glm::vec3(-1.0f, -1.0f,  1.0f)), 
                            glm::normalize(glm::vec3( 1.0f, -1.0f,  1.0f)),
                            glm::normalize(glm::vec3( 1.0f,  1.0f,  1.0f)),
                            glm::normalize(glm::vec3(-1.0f,  1.0f,  1.0f)),
                            
                            glm::normalize(glm::vec3(-1.0f, -1.0f, -1.0f)),
                            glm::normalize(glm::vec3( 1.0f, -1.0f, -1.0f)),
                            glm::normalize(glm::vec3( 1.0f,  1.0f, -1.0f)),
                            glm::normalize(glm::vec3(-1.0f,  1.0f, -1.0f))
                        };

    buffers.texcoords = { 
                            glm::vec2(0.0f, 0.0f),
                            glm::vec2(1.0f, 0.0f),
                            glm::vec2(1.0f, 1.0f),
                            glm::vec2(0.0f, 1.0f),
                        
                            glm::vec2(0.0f, 0.0f),
                            glm::vec2(1.0f, 0.0f),
                            glm::vec2(1.0f, 1.0f),
                            glm::vec2(0.0f, 1.0f)
                        };

    buffers.indices   = { 
                            0, 1, 2, 2, 3, 0, //front
                            1, 5, 6, 6, 2, 1, //right
                            7, 6, 5, 5, 4, 7, //back
                            4, 0, 3, 3, 7, 4, //left
                            3, 2, 6, 6, 7, 3, //top
                            4, 5, 1, 1, 0, 4  //bottom
                        };
}

void GeomPrimitive::genTorus(VEBuffers &buffers, float innerRadius, float outerRadius, unsigned int slices, unsigned int stacks)
{
    float phi   = 0.0f;
    float theta = 0.0f;

    float cos2PIp;
    float sin2PIp;
    float cos2PIt;
    float sin2PIt;

    float torusRadius = (outerRadius - innerRadius) * 0.5f;
    float centerRadius = outerRadius - torusRadius;

    float phiInc   = 1.0f / (float) slices;
    float thetaInc = 1.0f / (float) stacks;

    buffers.positions.reserve((stacks + 1) * (slices + 1));
    buffers.indices.reserve(stacks * slices * 2 * 3);

    for(unsigned int sideCount = 0; sideCount <= slices; ++sideCount, phi += phiInc)
    {
        cos2PIp = glm::cos(glm::two_pi<float>() * phi);
        sin2PIp = glm::sin(glm::two_pi<float>() * phi);

        theta = 0.0f;
        for(unsigned int faceCount = 0; faceCount <= stacks; ++faceCount, theta += thetaInc)
        {
            cos2PIt = glm::cos(glm::two_pi<float>() * theta);
            sin2PIt = glm::sin(glm::two_pi<float>() * theta);

            buffers.positions.push_back(glm::vec3((centerRadius + torusRadius * cos2PIt) * cos2PIp,
                                                  (centerRadius + torusRadius * cos2PIt) * sin2PIp,
                                                   torusRadius * sin2PIt));

            buffers.normals.push_back(glm::vec3(cos2PIp * cos2PIt, 
                                                sin2PIp * cos2PIt, 
                                                sin2PIt));

            buffers.texcoords.push_back(glm::vec2(phi, theta));
        }
    }

    for(unsigned int sideCount = 0; sideCount < slices; ++sideCount)
    {
        for(unsigned int faceCount = 0; faceCount < stacks; ++faceCount)
        {
            GLushort v0 =  sideCount      * (stacks + 1) + faceCount;
            GLushort v1 = (sideCount + 1) * (stacks + 1) + faceCount;
            GLushort v2 = (sideCount + 1) * (stacks + 1) + (faceCount + 1);
            GLushort v3 =  sideCount      * (stacks + 1) + (faceCount + 1);

            buffers.indices.push_back(v0);
            buffers.indices.push_back(v1);
            buffers.indices.push_back(v2);

            buffers.indices.push_back(v0);
            buffers.indices.push_back(v2);
            buffers.indices.push_back(v3);
        }
    }
 }

void GeomPrimitive::genCylinder(VEBuffers &buffers, float height, float r, unsigned int slices)
{
    float halfHeight = height * 0.5f;
    glm::vec3 p1 = glm::vec3(0.0f, halfHeight, 0.0f);
    glm::vec3 p2 = -p1;

    float thetaInc = glm::two_pi<float>() / (float) slices;
    float theta    = 0.0f;
    float sign     = -1.0f;

    /* Center bottom */
    buffers.positions.push_back(p2);
    buffers.normals.push_back  (glm::vec3(0.0f, -1.0f, 0.0f));
    buffers.texcoords.push_back(glm::vec2(0.0f, 0.0f));

    /* Bottom */
    for (unsigned int sideCount = 0; sideCount <= slices; ++sideCount, theta += thetaInc)
    {
        buffers.positions.push_back(glm::vec3(glm::cos(theta) * r, -halfHeight, -glm::sin(theta) * r));
        buffers.normals.push_back  (glm::vec3(0.0f, -1.0f, 0.0f));
        buffers.texcoords.push_back(glm::vec2(0.0f, 0.0f));
    }

    /* Center top */
    buffers.positions.push_back(p1);
    buffers.normals.push_back  (glm::vec3(0.0f, 1.0f, 0.0f));
    buffers.texcoords.push_back(glm::vec2(1.0f, 1.0f));

    /* Top */
    for (unsigned int sideCount = 0; sideCount <= slices; ++sideCount, theta += thetaInc)
    {
        buffers.positions.push_back(glm::vec3(glm::cos(theta) * r, halfHeight, -glm::sin(theta) * r));
        buffers.normals.push_back  (glm::vec3(0.0f, 1.0f, 0.0f));
        buffers.texcoords.push_back(glm::vec2(1.0f, 1.0f));
    }

    /* Sides */
    for (unsigned int sideCount = 0; sideCount <= slices; ++sideCount, theta += thetaInc)
    {
        sign = -1.0f;

        for(int i = 0; i < 2; ++i)
        {
            buffers.positions.push_back(glm::vec3(glm::cos(theta) * r, halfHeight * sign, -glm::sin(theta) * r));
            buffers.normals.push_back  (glm::vec3(glm::cos(theta), 0.0f, -glm::sin(theta)));
            buffers.texcoords.push_back(glm::vec2(sideCount / (float)slices, (sign + 1.0f) * 0.5f));

            sign = 1.0f;
        }
    }

    GLushort centerIdx = 0;
    GLushort idx       = 1;

    /* Indices Bottom */
    for (unsigned int sideCount = 0; sideCount < slices; ++sideCount)
    {
        buffers.indices.push_back(centerIdx);
        buffers.indices.push_back(idx + 1);
        buffers.indices.push_back(idx);

        ++idx;
    }
    ++idx;

    /* Indices Top */
    centerIdx = idx;
    ++idx;

    for (unsigned int sideCount = 0; sideCount < slices; ++sideCount)
    {
        buffers.indices.push_back(centerIdx);
        buffers.indices.push_back(idx);
        buffers.indices.push_back(idx + 1);

        ++idx;
    }
    ++idx;

    /* Indices Sides */
    for (unsigned int sideCount = 0; sideCount < slices; ++sideCount)
    {
        buffers.indices.push_back(idx);
        buffers.indices.push_back(idx + 2);
        buffers.indices.push_back(idx + 1);

        buffers.indices.push_back(idx + 2);
        buffers.indices.push_back(idx + 3);
        buffers.indices.push_back(idx + 1);

        idx += 2;
    }
}

void GeomPrimitive::genCone(VEBuffers &buffers, float height, float r, unsigned int slices, unsigned int stacks)
{
    float halfHeight = height * 0.5f;
    glm::vec3 p = glm::vec3(0.0f, halfHeight, 0.0f);

    float thetaInc = glm::two_pi<float>() / (float) slices;
    float theta    = 0.0f;
    float sign     = -1.0f;

    /* Center bottom */
    buffers.positions.push_back(-p);
    buffers.normals.push_back  (glm::vec3(0.0f, -1.0f, 0.0f));
    buffers.texcoords.push_back(glm::vec2(0.0f, 0.0f));

    /* Bottom */
    for (unsigned int sideCount = 0; sideCount <= slices; ++sideCount, theta += thetaInc)
    {
        buffers.positions.push_back(glm::vec3(glm::cos(theta) * r, -halfHeight, -glm::sin(theta) * r));
        buffers.normals.push_back  (glm::vec3(0.0f, -1.0f, 0.0f));
        buffers.texcoords.push_back(glm::vec2(0.0f, 0.0f));
    }

    /* Sides */
    float l = glm::sqrt(height * height + r * r);

    for (unsigned int stackCount = 0; stackCount <= stacks; ++stackCount)
    {
        float level = stackCount / (float) stacks;

        for (unsigned int sliceCount = 0; sliceCount <= slices; ++sliceCount, theta += thetaInc)
        {
            buffers.positions.push_back(glm::vec3(glm::cos(theta) * r * (1.0f - level), 
                                                  -halfHeight + 2.0f * halfHeight * level,
                                                  -glm::sin(theta) * r * (1.0f - level)));
            buffers.normals.push_back  (glm::vec3(glm::cos(theta) * height / l, r / l, -glm::sin(theta) * height / l));
            buffers.texcoords.push_back(glm::vec2(sliceCount / (float)slices, level));
        }
    }

    GLushort centerIdx = 0;
    GLushort idx       = 1;

    /* Indices Bottom */
    for (unsigned int sliceCount = 0; sliceCount < slices; ++sliceCount)
    {
        buffers.indices.push_back(centerIdx);
        buffers.indices.push_back(idx + 1);
        buffers.indices.push_back(idx);

        ++idx;
    }
    ++idx;

    /* Indices Sides */
    for (unsigned int stackCount = 0; stackCount < stacks; ++stackCount)
    {
        for (unsigned int sliceCount = 0; sliceCount < slices; ++sliceCount)
        {
            buffers.indices.push_back(idx);
            buffers.indices.push_back(idx + 1);
            buffers.indices.push_back(idx + slices + 1);

            buffers.indices.push_back(idx + 1);
            buffers.indices.push_back(idx + slices + 2);
            buffers.indices.push_back(idx + slices + 1);

            ++idx;
        }

        ++idx;
    }
}

void GeomPrimitive::genQuad(VEBuffers & buffers, float width, float height)
{
    float halfWidth  = width * 0.5f;
    float halfHeight = height * 0.5f;

    buffers.positions.push_back(glm::vec3(-halfWidth, 0.0f, -halfHeight));
    buffers.positions.push_back(glm::vec3(-halfWidth, 0.0f,  halfHeight));
    buffers.positions.push_back(glm::vec3( halfWidth, 0.0f, -halfHeight));
    buffers.positions.push_back(glm::vec3( halfWidth, 0.0f,  halfHeight));

    buffers.normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
    buffers.normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
    buffers.normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
    buffers.normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));

    buffers.texcoords.push_back(glm::vec2(0.0f, 1.0f));
    buffers.texcoords.push_back(glm::vec2(0.0f, 0.0f));
    buffers.texcoords.push_back(glm::vec2(1.0f, 1.0f));
    buffers.texcoords.push_back(glm::vec2(1.0f, 0.0f));

    buffers.indices.push_back(0);
    buffers.indices.push_back(1);
    buffers.indices.push_back(2);
    buffers.indices.push_back(3);
}

void GeomPrimitive::genPlane(VEBuffers & buffers, float width, float height, unsigned int slices, unsigned int stacks)
{
    float widthInc  = width  / (float) slices;
    float heightInc = height / (float) stacks;

    float w = -width * 0.5f;
    float h = -height * 0.5f;

    for (unsigned int j = 0; j <= stacks; ++j, h += heightInc)
    {
        for (unsigned int i = 0; i <= slices; ++i, w += widthInc)
        {
            buffers.positions.push_back(glm::vec3(w, 0.0f, h));
            buffers.normals.push_back  (glm::vec3(0.0f, 1.0f, 0.0f));
            buffers.texcoords.push_back(glm::vec2(i / (float) slices, j / (float) stacks));
        }
        w = -width * 0.5f;
    }

    GLushort idx = 0;

    for (unsigned int j = 0; j < stacks; ++j)
    {
        for (unsigned int i = 0; i < slices; ++i)
        {
            buffers.indices.push_back(idx);
            buffers.indices.push_back(idx + slices + 1);
            buffers.indices.push_back(idx + 1);

            buffers.indices.push_back(idx + 1);
            buffers.indices.push_back(idx + slices + 1);
            buffers.indices.push_back(idx + slices + 2);

            ++idx;
        }

        ++idx;
    }
}
