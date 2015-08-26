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

//TODO: gen indices + change drawing func to glDrawElements
void GeomPrimitive::genCylinder(VEBuffers &buffers, float height, float r, unsigned int slices)
{
    glm::vec3 p1 = glm::vec3(0.0f) + glm::vec3(0.0f, height * 0.5f, 0.0f);
    glm::vec3 p2 = p1 - glm::vec3(0.0f, height, 0.0f);

    glm::vec3 n = p2 - p1;
    glm::vec3 a = glm::vec3(n.x + 1.0f, n.y, n.z);

    glm::vec3 b = glm::cross(a, n);
              a = glm::cross(n, b);

    a = glm::normalize(a);
    b = glm::normalize(b);

    glm::vec3 p;

    float theta = 0.0f;
    float thetaInc = glm::two_pi<float>() / (float) slices;
    
    GLushort idx = 0;

    for (unsigned int sideCount = 0; sideCount <= slices; ++sideCount, theta += thetaInc)
    {
        n = glm::cos(theta) * a + glm::sin(theta) * b;
        n = glm::normalize(n);

        p = p2 + r * n;
        buffers.positions.push_back(p);
        buffers.normals.push_back(n);
        buffers.texcoords.push_back(glm::vec2(sideCount / (float) slices, 1.0f));
        buffers.indices.push_back(idx++);

        p = p1 + r * n;
        buffers.positions.push_back(p);
        buffers.normals.push_back(n);
        buffers.texcoords.push_back(glm::vec2(sideCount / (float)slices, 0.0f));
        buffers.indices.push_back(idx++);
    }
}


