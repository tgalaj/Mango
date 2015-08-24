#include "GeomPrimitive.h"

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


