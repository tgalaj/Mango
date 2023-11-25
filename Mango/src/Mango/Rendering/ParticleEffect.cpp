#include "mgpch.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/vec4.hpp>

#include "ParticleEffect.h"

namespace mango
{
    ParticleEffect::ParticleEffect(GLuint maxParticles) 
        : color(glm::vec4(0.85f, 0.325f, 0.0f, 0.2f)),
          simulate(true)
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("ParticleEffect::ParticleEffect");

        if (maxParticles > 18000000)
        {
            maxParticles = 18000000;
        }

        if (maxParticles < 1000)
        {
            maxParticles = 1000;
        }

        m_maxParticles = maxParticles;

        m_initPositions = new GLfloat[m_maxParticles * 4];
        m_initVelocities = new GLfloat[m_maxParticles * 4];

        memset(m_initPositions, 0, sizeof(GLfloat) * 4 * m_maxParticles);
        memset(m_initVelocities, 0, sizeof(GLfloat) * 4 * m_maxParticles);

        glm::vec4 p(0.0f, 0.0f, 0.0f, 1.0f);
        int particlesCountPerDimension = (int)cbrt(m_maxParticles);

        glm::mat4 transf = glm::translate(glm::mat4(1.0f), glm::vec3(-1, -1, -1));

        float dx = 2.0f / (particlesCountPerDimension - 1),
              dy = 2.0f / (particlesCountPerDimension - 1),
              dz = 2.0f / (particlesCountPerDimension - 1);

        int counter = 0;
        for (int i = 0; i < particlesCountPerDimension; ++i)
        {
            for (int j = 0; j < particlesCountPerDimension; ++j)
            {
                for (int k = 0; k < particlesCountPerDimension; ++k)
                {
                    p.x = dx * i;
                    p.y = dy * j;
                    p.z = dz * k;
                    p.w = 1.0f;

                    p = transf * p;

                    m_initPositions[counter++] = p.x;
                    m_initPositions[counter++] = p.y;
                    m_initPositions[counter++] = p.z;
                    m_initPositions[counter++] = p.w;
                }
            }
        }

        /* VBOs */
        glGenBuffers(2, m_vbos);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_vbos[POSITIONS]);
        glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(GLfloat) * 4 * m_maxParticles, m_initPositions, GL_DYNAMIC_STORAGE_BIT /*flags*/);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_vbos[VELOCITIES]);
        glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(GLfloat) * 4 * m_maxParticles, m_initVelocities, GL_DYNAMIC_STORAGE_BIT /*flags*/);

        /* VAO */
        glCreateVertexArrays(1, &m_vao);
        glEnableVertexArrayAttrib(m_vao, 0);

        glVertexArrayAttribBinding(m_vao, 0, 0);
        glVertexArrayAttribFormat(m_vao, 0, 4, GL_FLOAT, GL_FALSE, 0);
        glVertexArrayVertexBuffer(m_vao, 0, m_vbos[POSITIONS], 0, sizeof(GLfloat) * 4);

        glPointSize(1.0f);
    }


    ParticleEffect::~ParticleEffect()
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("ParticleEffect::~ParticleEffect");

        delete[] m_initPositions;
        delete[] m_initVelocities;

        for (auto & vbo_id : m_vbos)
        {
            if (vbo_id != 0)
            {
                glDeleteBuffers(1, &vbo_id);
                vbo_id = 0;
            }
        }

        if (m_vao != 0)
        {
            glDeleteVertexArrays(1, &m_vao);
            m_vao = 0;
        }
    }

    void ParticleEffect::reset()
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("ParticleEffect::reset");

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_vbos[POSITIONS]);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLfloat) * 4 * m_maxParticles, m_initPositions);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_vbos[VELOCITIES]);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLfloat) * 4 * m_maxParticles, m_initVelocities);

        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);
    }

    void ParticleEffect::render(CameraComponent * cam)
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("ParticleEffect::render");

        if (simulate)
        {
            //m_shader = ShaderManager::getShader("ve_compute_particles");
            m_shader->bind();

            glDispatchCompute(m_maxParticles / 1000, 1, 1);
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        }
        /* Use color shader for particles */
        //m_shader = ShaderManager::getShader("ve_render_particles");
        m_shader->bind();

        //m_shader->setUniformMatrix4fv("viewProj", cam->getViewProjection());
        m_shader->setUniform("color", color);

        glBindVertexArray(m_vao);
        glDrawArrays(GL_POINTS, 0, m_maxParticles);
    }
}