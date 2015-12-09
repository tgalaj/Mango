#include "ParticleEffect.h"
#include "ShaderManager.h"

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <math.h>
#include <vector>

ParticleEffect::ParticleEffect(GLuint _max_particles)
    : color(glm::vec4(0.85f, 0.325f, 0.0f, 0.4f)),
      simulate(true)
{
    if(_max_particles > 18000000)
    {
        _max_particles = 18000000;
    }

    max_particles = _max_particles;

    init_positions  = new GLfloat[max_particles * 4];
    init_velocities = new GLfloat[max_particles * 4];
    
    memset(init_positions,  0, sizeof(GLfloat) * 4 * max_particles);
    memset(init_velocities, 0, sizeof(GLfloat) * 4 * max_particles);

    glm::vec4 p(0.0f, 0.0f, 0.0f, 1.0f);
    int no_particles_in_dim = cbrt(max_particles);

    glm::mat4 transf = glm::translate(glm::mat4(1.0f), glm::vec3(-1, -1, -1));

    float dx = 2.0f / (no_particles_in_dim - 1),
          dy = 2.0f / (no_particles_in_dim - 1),
          dz = 2.0f / (no_particles_in_dim - 1);

    int counter = 0;
    for (int i = 0; i < no_particles_in_dim; ++i)
    {
        for (int j = 0; j < no_particles_in_dim; ++j)
        {
            for (int k = 0; k < no_particles_in_dim; ++k)
            {
                p.x = dx * i;
                p.y = dy * j;
                p.z = dz * k;
                p.w = 1.0f;

                p = transf * p;

                init_positions[counter++] = p.x;
                init_positions[counter++] = p.y;
                init_positions[counter++] = p.z;
                init_positions[counter++] = p.w;
            }
        }
    }

    /* VBOs */
    glGenBuffers(2, vbo_ids);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vbo_ids[POSITIONS]);
    glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(GLfloat) * 4 * max_particles, init_positions, GL_DYNAMIC_STORAGE_BIT /*flags*/);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, vbo_ids[VELOCITIES]);
    glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(GLfloat) * 4 * max_particles, init_velocities, GL_DYNAMIC_STORAGE_BIT /*flags*/);

    /* VAO */
    glCreateVertexArrays(1, &vao_id);
    glEnableVertexArrayAttrib(vao_id, 0);

    glVertexArrayAttribBinding(vao_id, 0, 0);
    glVertexArrayAttribFormat(vao_id, 0, 4, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayVertexBuffer(vao_id, 0, vbo_ids[POSITIONS], 0, sizeof(GLfloat) * 4);

    glPointSize(1.0f);
}


ParticleEffect::~ParticleEffect()
{
    delete[] init_positions;
    delete[] init_velocities;

    for (auto & vbo_id : vbo_ids)
    {
        if (vbo_id != 0)
        {
            glDeleteBuffers(1, &vbo_id);
            vbo_id = 0;
        }
    }

    if (vao_id != 0)
    {
        glDeleteVertexArrays(1, &vao_id);
        vao_id = 0;
    }
}

void ParticleEffect::reset()
{
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vbo_ids[POSITIONS]);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLfloat) * 4 * max_particles, init_positions);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, vbo_ids[VELOCITIES]);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLfloat) * 4 * max_particles, init_velocities);

    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);
}

void ParticleEffect::render(Camera * cam)
{
    if(simulate)
    {
        shader = ShaderManager::getShader("ve_compute_particles");
        shader->apply();
    
        glDispatchCompute(max_particles / 1000, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }
    /* Use color shader for particles */
    shader = ShaderManager::getShader("ve_render_particles");
    shader->apply();

    shader->setUniformMatrix4fv("viewProj", cam->getViewProjection());
    shader->setUniform4fv("color", color);

    glBindVertexArray(vao_id);
    glDrawArrays(GL_POINTS, 0, max_particles);
}