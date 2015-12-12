#pragma once

#include <GL\glew.h>

#include "Camera.h"
#include "SceneNode.h"
#include "Shader.h"

class Cloth : public SceneNode
{
public:
    friend class Renderer;

    Cloth(int particles_x, int particles_y, float cloth_size_x, float cloth_size_y);
    ~Cloth();

    bool simulate;

    void reset();

private:
    GLfloat * init_positions;
    GLfloat * init_velocities;

    Shader * compute_cloth_shader;
    Shader * compute_cloth_normals_shader;

    glm::vec2 particles_dim;
    glm::vec2 cloth_size;

    GLuint vbo_ids[7];
    GLuint vao_id;
    GLuint num_elements;
    GLchar read_buf;

    const GLuint PRIM_RESTART;

    enum { POSITIONS_0, POSITIONS_1, VELOCITIES_0, VELOCITIES_1, NORMALS, INDICES, TEXCOORDS };

    void compute();
    void render(Shader * shader);
};