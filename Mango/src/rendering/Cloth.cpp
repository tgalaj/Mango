#include "mgpch.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"

#include "rendering/Cloth.h"
#include "core_engine/CoreAssetManager.h"

namespace mango
{
    Cloth::Cloth(int particles_x, int particles_y, float cloth_size_x, float cloth_size_y)
        : m_particles_dim(particles_x, particles_y),
        m_cloth_size(cloth_size_x, cloth_size_y),
        PRIM_RESTART(0xffffff),
        m_read_buf(0),
        m_simulate(true),
        m_gravity(glm::vec3(0.0f, 10 * -9.81f, 0.0f)),
        m_particle_mass(0.1f / 1.5f),
        m_spring_k(4000.0f),
        m_delta_t(0.00005f*2.0f),
        m_damping(20.0f*0.1f),
        m_should_self_collide(1)
    {
        for (auto & pin : m_pins)
        {
            pin = true;
        }

        if (m_particles_dim.x * m_particles_dim.y > 18000000)
        {
            m_particles_dim.x = 4240;
            m_particles_dim.y = 4240;
        }

        if (m_particles_dim.x <= 0 || m_particles_dim.y <= 0)
        {
            m_particles_dim.x = 10;
            m_particles_dim.y = 10;
        }

        m_init_positions = new GLfloat[(int)m_particles_dim.x * (int)m_particles_dim.y * 4];
        m_init_velocities = new GLfloat[(int)m_particles_dim.x * (int)m_particles_dim.y * 4];

        std::vector<GLfloat> init_tc;
        std::vector<GLuint>  init_el;

        memset(m_init_positions, 0, sizeof(GLfloat) * 4 * (size_t)m_particles_dim.x * (size_t)m_particles_dim.y);
        memset(m_init_velocities, 0, sizeof(GLfloat) * 4 * (size_t)m_particles_dim.x * (size_t)m_particles_dim.y);

        glm::mat4 transf = glm::translate(glm::mat4(1.0), glm::vec3(0, m_cloth_size.y, 0));
        transf = glm::rotate(transf, glm::radians(80.0f), glm::vec3(1, 0, 0));
        transf = glm::translate(transf, glm::vec3(0, -m_cloth_size.y, 0));

        glm::vec4 p(0.0f, 0.0f, 0.0f, 1.0f);

        float dx = m_cloth_size.x / (m_particles_dim.x - 1),
            dy = m_cloth_size.y / (m_particles_dim.y - 1),
            ds = 1.0f / (m_particles_dim.x - 1),
            dt = 1.0f / (m_particles_dim.y - 1);

        m_rest_len_x = dx;
        m_rest_len_y = dy;
        m_rest_len_diag = glm::sqrt(dx * dx + dy * dy);

        int counter = 0;
        for (int i = 0; i < m_particles_dim.y; ++i)
        {
            for (int j = 0; j < m_particles_dim.x; ++j)
            {
                p.x = dx * j;
                p.y = dy * i;
                p.z = 0.0f;

                p = transf * p;

                m_init_positions[counter++] = p.x;
                m_init_positions[counter++] = p.y;
                m_init_positions[counter++] = p.z;
                m_init_positions[counter++] = p.w;

                init_tc.push_back(ds * j);
                init_tc.push_back(dt * i);
            }
        }

        for (int row = 0; row < m_particles_dim.y - 1; ++row)
        {
            for (int col = 0; col < m_particles_dim.x; ++col)
            {
                init_el.push_back((row + 1) * (unsigned)m_particles_dim.x + col);
                init_el.push_back(row      * (unsigned)m_particles_dim.x + col);
            }
            init_el.push_back(PRIM_RESTART);
        }

        glEnable(GL_PRIMITIVE_RESTART);
        glPrimitiveRestartIndex(PRIM_RESTART);

        /* VBOs */
        GLuint no_parts = (GLuint)m_particles_dim.x * (GLuint)m_particles_dim.y;

        glGenBuffers(7, m_vbo_ids);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_vbo_ids[POSITIONS_0]);
        glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(GLfloat) * 4 * no_parts, m_init_positions, GL_DYNAMIC_STORAGE_BIT /*flags*/);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_vbo_ids[POSITIONS_1]);
        glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(GLfloat) * 4 * no_parts, NULL, GL_DYNAMIC_STORAGE_BIT /*flags*/);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_vbo_ids[VELOCITIES_0]);
        glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(GLfloat) * 4 * no_parts, m_init_velocities, GL_DYNAMIC_STORAGE_BIT /*flags*/);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_vbo_ids[VELOCITIES_1]);
        glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(GLfloat) * 4 * no_parts, NULL, GL_DYNAMIC_STORAGE_BIT /*flags*/);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, m_vbo_ids[NORMALS]);
        glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(GLfloat) * 4 * no_parts, NULL, GL_DYNAMIC_STORAGE_BIT /*flags*/);

        glBindBuffer(GL_ARRAY_BUFFER, m_vbo_ids[INDICES]);
        glBufferStorage(GL_ARRAY_BUFFER, sizeof(GLuint) * init_el.size(), &init_el[0], GL_DYNAMIC_STORAGE_BIT /*flags*/);

        glBindBuffer(GL_ARRAY_BUFFER, m_vbo_ids[TEXCOORDS]);
        glBufferStorage(GL_ARRAY_BUFFER, sizeof(GLfloat) * init_tc.size(), &init_tc[0], GL_DYNAMIC_STORAGE_BIT /*flags*/);

        m_num_elements = init_el.size();

        /* Set up VAO */
        glCreateVertexArrays(1, &m_vao_id);

        glEnableVertexArrayAttrib(m_vao_id, 0 /*index*/);
        glEnableVertexArrayAttrib(m_vao_id, 1 /*index*/);
        glEnableVertexArrayAttrib(m_vao_id, 2 /*index*/);

        glVertexArrayElementBuffer(m_vao_id, m_vbo_ids[INDICES]);

        /* Separate attribute format */
        glVertexArrayAttribFormat(m_vao_id, 0 /*index*/, 4 /*size*/, GL_FLOAT, GL_FALSE, 0 /*relativeoffset*/);
        glVertexArrayAttribFormat(m_vao_id, 1 /*index*/, 4 /*size*/, GL_FLOAT, GL_FALSE, 0 /*relativeoffset*/);
        glVertexArrayAttribFormat(m_vao_id, 2 /*index*/, 2 /*size*/, GL_FLOAT, GL_FALSE, 0 /*relativeoffset*/);

        glVertexArrayAttribBinding(m_vao_id, 0 /*index*/, 0 /*bindingindex*/);
        glVertexArrayAttribBinding(m_vao_id, 1 /*index*/, 1 /*bindingindex*/);
        glVertexArrayAttribBinding(m_vao_id, 2 /*index*/, 2 /*bindingindex*/);

        glVertexArrayVertexBuffer(m_vao_id, 0 /*bindingindex*/, m_vbo_ids[POSITIONS_0], 0 /*offset*/, sizeof(glm::vec4) /*stride*/);
        glVertexArrayVertexBuffer(m_vao_id, 1 /*bindingindex*/, m_vbo_ids[NORMALS], 0 /*offset*/, sizeof(glm::vec4) /*stride*/);
        glVertexArrayVertexBuffer(m_vao_id, 2 /*bindingindex*/, m_vbo_ids[TEXCOORDS], 0 /*offset*/, sizeof(glm::vec2) /*stride*/);

        /* Get shaders */
        //m_compute_cloth_shader = ShaderManager::getShader("ve_compute_cloth");
        //m_compute_cloth_normals_shader = ShaderManager::getShader("ve_compute_cloth_normals");

        /* Set up textures and material */
//        Texture * diff = CoreAssetManager::genTexture2D("assets/texture/white_4x4.jpg");
       // diff->setTypeName(std::string("texture_diffuse"));

        //Texture * spec = CoreAssetManager::genTexture2D("assets/texture/spec_default.jpg");
       // spec->setTypeName(std::string("texture_specular"));

       // m_textures.push_back(diff);
       // m_textures.push_back(spec);
       // m_textures.push_back(spec);

        /* Compute normals */
        m_compute_cloth_normals_shader->bind();
        glDispatchCompute((GLuint)m_particles_dim.x / 10, (GLuint)m_particles_dim.y / 10, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }

    Cloth::~Cloth()
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

    void Cloth::reset()
    {
        m_read_buf = 0;

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_vbo_ids[POSITIONS_0]);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLfloat) * 4 * (GLsizeiptr)m_particles_dim.x * (GLsizeiptr)m_particles_dim.y, m_init_positions);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_vbo_ids[POSITIONS_1]);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLfloat) * 4 * (GLsizeiptr)m_particles_dim.x * (GLsizeiptr)m_particles_dim.y, NULL);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_vbo_ids[VELOCITIES_0]);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLfloat) * 4 * (GLsizeiptr)m_particles_dim.x * (GLsizeiptr)m_particles_dim.y, m_init_velocities);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_vbo_ids[VELOCITIES_1]);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLfloat) * 4 * (GLsizeiptr)m_particles_dim.x * (GLsizeiptr)m_particles_dim.y, NULL);

        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);

        m_compute_cloth_normals_shader->bind();
        glDispatchCompute((GLuint)m_particles_dim.x / 10, (GLuint)m_particles_dim.y / 10, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }

    void Cloth::setDiffuseTexture(const std::string & filename)
    {
        //Texture * diff = CoreAssetManager::genTexture2D(filename);
        //diff->setTypeName(std::string("texture_diffuse"));

        //m_textures[0] = diff;
    }

    void Cloth::setSpecularTexture(const std::string & filename)
    {
       /* Texture * spec = CoreAssetManager::genTexture2D(filename);
        spec->setTypeName(std::string("texture_specular"));*/

       // m_textures[1] = spec;
    }

    void Cloth::setColor(glm::vec3 & color)
    {
        //m_material.m_diffuse_color = color;
    }

    void Cloth::setShininess(float shinines)
    {
        //m_material.m_shininess = shinines;
    }

    void Cloth::setPin(int idx, bool isActive)
    {
        if (idx < 0)
        {
            idx = 0;
        }

        if (idx >= sizeof(m_pins) / sizeof(int))
        {
            idx = sizeof(m_pins) / sizeof(int) - 1;
        }

        m_pins[idx] = isActive;
    }

    void Cloth::compute()
    {
        if (m_simulate)
        {
            m_compute_cloth_shader->bind();

            m_compute_cloth_shader->setUniform("pins", 5, &m_pins[0]);
            m_compute_cloth_shader->setUniform("Gravity", m_gravity);
            m_compute_cloth_shader->setUniform("ParticleMass", m_particle_mass);
            m_compute_cloth_shader->setUniform("ParticleInvMass", 1.0f / m_particle_mass);
            m_compute_cloth_shader->setUniform("RestLengthHoriz", m_rest_len_x);
            m_compute_cloth_shader->setUniform("RestLengthVert", m_rest_len_y);
            m_compute_cloth_shader->setUniform("RestLengthDiag", m_rest_len_diag);
            m_compute_cloth_shader->setUniform("SpringK", m_spring_k);
            m_compute_cloth_shader->setUniform("DeltaT", m_delta_t);
            m_compute_cloth_shader->setUniform("DampingConst", m_damping);
            m_compute_cloth_shader->setUniform("shouldSelfCollide", m_should_self_collide);

//            m_compute_cloth_shader->setUniformMatrix4fv("model", m_world_transform);

            for (int i = 0; i < 50; ++i)
            {
                glDispatchCompute((GLuint)m_particles_dim.x / 10, (GLuint)m_particles_dim.y / 10, 1);
                glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

                /* Swap buffers */
                m_read_buf = 1 - m_read_buf;
                glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_vbo_ids[m_read_buf]);
                glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_vbo_ids[1 - m_read_buf]);
                glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_vbo_ids[m_read_buf + 2]);
                glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_vbo_ids[1 - m_read_buf + 2]);
            }

            m_compute_cloth_normals_shader->bind();
            glDispatchCompute((GLuint)m_particles_dim.x / 10, (GLuint)m_particles_dim.y / 10, 1);
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        }
    }

    void Cloth::render(Shader * shader)
    {
        //shader->setUniformMatrix3fv("normalMatrix", m_normal_matrix);
       // shader->setUniformMatrix4fv("world", m_world_transform);

        for (GLuint i = 0; i < m_textures.size(); ++i)
        {
            m_textures[i]->bind(i);
        }

        //shader->setUniform1f("material.shininess", m_material.m_shininess);
        //shader->setUniform3fv("material.diffuseColor", m_material.m_diffuse_color);

        glBindVertexArray(m_vao_id);
        glDrawElements(GL_TRIANGLE_STRIP, m_num_elements, GL_UNSIGNED_INT, NULL);
    }
}
