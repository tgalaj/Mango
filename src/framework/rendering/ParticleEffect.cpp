#include "framework/rendering/ParticleEffect.h"

#include <glm/vec4.hpp>
#include <glm\gtc\matrix_transform.hpp>

namespace Vertex
{
    ParticleEffect::ParticleEffect(GLuint _max_particles)
        : m_color(glm::vec4(0.85f, 0.325f, 0.0f, 0.2f)),
        m_simulate(true)
    {
        if (_max_particles > 18000000)
        {
            _max_particles = 18000000;
        }

        if (_max_particles < 1000)
        {
            _max_particles = 1000;
        }

        m_max_particles = _max_particles;

        m_init_positions = new GLfloat[m_max_particles * 4];
        m_init_velocities = new GLfloat[m_max_particles * 4];

        memset(m_init_positions, 0, sizeof(GLfloat) * 4 * m_max_particles);
        memset(m_init_velocities, 0, sizeof(GLfloat) * 4 * m_max_particles);

        glm::vec4 p(0.0f, 0.0f, 0.0f, 1.0f);
        int no_particles_in_dim = (int)cbrt(m_max_particles);

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

                    m_init_positions[counter++] = p.x;
                    m_init_positions[counter++] = p.y;
                    m_init_positions[counter++] = p.z;
                    m_init_positions[counter++] = p.w;
                }
            }
        }

        /* VBOs */
        glGenBuffers(2, m_vbo_ids);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_vbo_ids[POSITIONS]);
        glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(GLfloat) * 4 * m_max_particles, m_init_positions, GL_DYNAMIC_STORAGE_BIT /*flags*/);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_vbo_ids[VELOCITIES]);
        glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(GLfloat) * 4 * m_max_particles, m_init_velocities, GL_DYNAMIC_STORAGE_BIT /*flags*/);

        /* VAO */
        glCreateVertexArrays(1, &m_vao_id);
        glEnableVertexArrayAttrib(m_vao_id, 0);

        glVertexArrayAttribBinding(m_vao_id, 0, 0);
        glVertexArrayAttribFormat(m_vao_id, 0, 4, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayVertexBuffer(m_vao_id, 0, m_vbo_ids[POSITIONS], 0, sizeof(GLfloat) * 4);

        glPointSize(1.0f);
    }


    ParticleEffect::~ParticleEffect()
    {
        delete[] m_init_positions;
        delete[] m_init_velocities;

        for (auto & vbo_id : m_vbo_ids)
        {
            if (vbo_id != 0)
            {
                glDeleteBuffers(1, &vbo_id);
                vbo_id = 0;
            }
        }

        if (m_vao_id != 0)
        {
            glDeleteVertexArrays(1, &m_vao_id);
            m_vao_id = 0;
        }
    }

    void ParticleEffect::reset()
    {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_vbo_ids[POSITIONS]);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLfloat) * 4 * m_max_particles, m_init_positions);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_vbo_ids[VELOCITIES]);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLfloat) * 4 * m_max_particles, m_init_velocities);

        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);
    }

    void ParticleEffect::render(CameraComponent * cam)
    {
        if (m_simulate)
        {
            //m_shader = ShaderManager::getShader("ve_compute_particles");
            m_shader->bind();

            glDispatchCompute(m_max_particles / 1000, 1, 1);
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        }
        /* Use color shader for particles */
        //m_shader = ShaderManager::getShader("ve_render_particles");
        m_shader->bind();

        //m_shader->setUniformMatrix4fv("viewProj", cam->getViewProjection());
        m_shader->setUniform("color", m_color);

        glBindVertexArray(m_vao_id);
        glDrawArrays(GL_POINTS, 0, m_max_particles);
    }
}