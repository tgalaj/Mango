#include "GeomPrimitive.h"

void GeomPrimitive::genCube(std::vector<glm::vec3> &positions, std::vector<glm::vec3> &normals, std::vector<glm::vec2> &texcoords, std::vector<GLubyte> &indices, float radius)
{
    float r2 = radius * 0.5f;

    positions = { 
                    glm::vec3(-r2, -r2,  r2), 
                    glm::vec3( r2, -r2,  r2),
                    glm::vec3( r2,  r2,  r2),
                    glm::vec3(-r2,  r2,  r2),     
                    
                    glm::vec3( r2, -r2, -r2),
                    glm::vec3(-r2, -r2, -r2),
                    glm::vec3(-r2,  r2, -r2),
                    glm::vec3( r2,  r2, -r2)
                };

    normals   = { 
                    glm::vec3(), 
                    glm::vec3(),
                    glm::vec3(),
                    glm::vec3(),

                    glm::vec3(),
                    glm::vec3(),
                    glm::vec3(),
                    glm::vec3()
                };

    texcoords = { 
                    glm::vec2(0.0f, 0.0f),
                    glm::vec2(1.0f, 0.0f),
                    glm::vec2(0.0f, 1.0f),
                    glm::vec2(1.0f, 1.0f),

                    glm::vec2(0.0f, 0.0f),
                    glm::vec2(1.0f, 0.0f),
                    glm::vec2(0.0f, 1.0f),
                    glm::vec2(1.0f, 1.0f)
                };

    indices   = { 
                    1, 2, 3, 3, 2, 4, //front
                    2, 5, 4, 4, 5, 8, //right
                    5, 6, 8, 8, 6, 7, //back
                    6, 1, 7, 7, 1, 3, //left
                    3, 4, 7, 7, 4, 8, //up
                    6, 5, 1, 1, 5, 2  //bottom
                };
}


