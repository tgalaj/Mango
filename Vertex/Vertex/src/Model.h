#pragma once

#include <GL/glew.h>
#include <glm\glm.hpp>
#include <string>
#include <vector>

struct VEBuffers
{
    std::vector<glm::vec3> positions, normals;
    std::vector<glm::vec2> texcoords; 
    std::vector<GLushort>   indices;
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
    void genTorus(float innerRadius = 1.0f, float outerRadius = 2.0f, unsigned int slices = 10, unsigned int stacks = 10);
    void genCone(float height = 3.0f, float r = 1.5f, unsigned int slices = 10, unsigned int stacks = 10);
    void genSphere(float radius = 1.5f, unsigned int slices = 12);
    void genCylinder(float height = 3.0f, float radius = 1.5f, unsigned int slices = 10);
    void genPlane(float width = 1.0f, float height = 1.0f, unsigned int slices = 5, unsigned int stacks = 5);
    void genQuad(float width = 1.0f, float height = 1.0f);

private:
    GLuint vao_id;
    GLuint vbo_ids[4];
    GLuint indices_count;
    GLenum draw_mode;

    enum { POSITION, NORMAL, TEXCOORD, INDEX };

    void setBuffers(VEBuffers &buffers);
};

