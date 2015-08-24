#pragma once

#include <GL/glew.h>
#include <glm\glm.hpp>
#include <string>
#include <vector>

struct VEBuffers
{
    std::vector<glm::vec3> positions, normals;
    std::vector<glm::vec2> texcoords; 
    std::vector<GLubyte>   indices;
};

class Model
{
public:
    Model();
    Model(std::string filename);
    virtual ~Model();

    void render();

    /* Primitives */
    void genCube(float radius = 1.0f);
    void genTorus();
    void genCone();
    void genSphere();
    void genCylinder();
    void genPlane();
    void genQuad();

private:
    GLuint vao_id;
    GLuint vbo_ids[4];
    GLuint indices_count;
    GLenum draw_mode;

    enum { POSITION, NORMAL, TEXCOORD, INDEX };

    void setBuffers(VEBuffers &buffers);
};

