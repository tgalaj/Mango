#pragma once

#include <GL\glew.h>

#include "Camera.h"
#include "SceneNode.h"
#include "Shader.h"

class ParticleEffect : public SceneNode
{
public:
    friend class Renderer;

    ParticleEffect(GLuint _max_particles);
    ~ParticleEffect();

    glm::vec4 color;
    bool simulate;

    void reset();

private:
    std::vector<GLfloat> init_positions;
    std::vector<GLfloat> init_velocities;

    Shader * shader;

    GLuint vbo_ids[2];
    GLuint vao_id;

    GLuint max_particles;

    enum { POSITIONS, VELOCITIES };

    void render(Camera * cam);
};

