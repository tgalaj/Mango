#pragma once

#include <GL/glew.h>
#include <string>

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
};

