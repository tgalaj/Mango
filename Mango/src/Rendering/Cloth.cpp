#include "mgpch.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/mat4x4.hpp"
#include "glm/vec3.hpp"

#include "Cloth.h"
#include "CoreEngine/CoreAssetManager.h"

namespace mango
{
    Cloth::Cloth(int particles_x, int particles_y, float cloth_size_x, float cloth_size_y)
        : m_particlesSize  (particles_x, particles_y),
          m_clothSize      (cloth_size_x, cloth_size_y),
          m_readBuf        (0),
          simulate         (true),
          gravity          (glm::vec3(0.0f, 10 * -9.81f, 0.0f)),
          particleMass     (0.1f / 1.5f),
          springK          (4000.0f),
          deltaTime        (0.00005f*2.0f),
          damping          (20.0f*0.1f),
          shouldSelfCollide(1),
          PRIM_RESTART     (0xffffff)
    {
        for (auto & pin : m_pins)
        {
            pin = true;
        }

        if (m_particlesSize.x * m_particlesSize.y > 18000000)
        {
            m_particlesSize.x = 4240;
            m_particlesSize.y = 4240;
        }

        if (m_particlesSize.x <= 0 || m_particlesSize.y <= 0)
        {
            m_particlesSize.x = 10;
            m_particlesSize.y = 10;
        }

        m_initPositions = new GLfloat[(int)m_particlesSize.x * (int)m_particlesSize.y * 4];
        m_initVelocities = new GLfloat[(int)m_particlesSize.x * (int)m_particlesSize.y * 4];

        std::vector<GLfloat> initTc;
        std::vector<GLuint>  initEl;

        memset(m_initPositions, 0, sizeof(GLfloat) * 4 * (size_t)m_particlesSize.x * (size_t)m_particlesSize.y);
        memset(m_initVelocities, 0, sizeof(GLfloat) * 4 * (size_t)m_particlesSize.x * (size_t)m_particlesSize.y);

        glm::mat4 transf = glm::translate(glm::mat4(1.0), glm::vec3(0, m_clothSize.y, 0));
                  transf = glm::rotate(transf, glm::radians(80.0f), glm::vec3(1, 0, 0));
                  transf = glm::translate(transf, glm::vec3(0, -m_clothSize.y, 0));

        glm::vec4 p(0.0f, 0.0f, 0.0f, 1.0f);

        float dx = m_clothSize.x / (m_particlesSize.x - 1),
              dy = m_clothSize.y / (m_particlesSize.y - 1),
              ds = 1.0f / (m_particlesSize.x - 1),
              dt = 1.0f / (m_particlesSize.y - 1);

        m_xRestLen    = dx;
        m_yRestLen    = dy;
        m_diagRestLen = glm::sqrt(dx * dx + dy * dy);

        int counter = 0;
        for (int i = 0; i < m_particlesSize.y; ++i)
        {
            for (int j = 0; j < m_particlesSize.x; ++j)
            {
                p.x = dx * j;
                p.y = dy * i;
                p.z = 0.0f;

                p = transf * p;

                m_initPositions[counter++] = p.x;
                m_initPositions[counter++] = p.y;
                m_initPositions[counter++] = p.z;
                m_initPositions[counter++] = p.w;

                initTc.push_back(ds * j);
                initTc.push_back(dt * i);
            }
        }

        for (int row = 0; row < m_particlesSize.y - 1; ++row)
        {
            for (int col = 0; col < m_particlesSize.x; ++col)
            {
                initEl.push_back((row + 1) * (unsigned)m_particlesSize.x + col);
                initEl.push_back(row      * (unsigned)m_particlesSize.x + col);
            }
            initEl.push_back(PRIM_RESTART);
        }

        glEnable(GL_PRIMITIVE_RESTART);
        glPrimitiveRestartIndex(PRIM_RESTART);

        /* VBOs */
        GLuint noParts = (GLuint)m_particlesSize.x * (GLuint)m_particlesSize.y;

        glGenBuffers(7, m_vbos);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_vbos[POSITIONS_0]);
        glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(GLfloat) * 4 * noParts, m_initPositions, GL_DYNAMIC_STORAGE_BIT /*flags*/);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_vbos[POSITIONS_1]);
        glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(GLfloat) * 4 * noParts, NULL, GL_DYNAMIC_STORAGE_BIT /*flags*/);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_vbos[VELOCITIES_0]);
        glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(GLfloat) * 4 * noParts, m_initVelocities, GL_DYNAMIC_STORAGE_BIT /*flags*/);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_vbos[VELOCITIES_1]);
        glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(GLfloat) * 4 * noParts, NULL, GL_DYNAMIC_STORAGE_BIT /*flags*/);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, m_vbos[NORMALS]);
        glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(GLfloat) * 4 * noParts, NULL, GL_DYNAMIC_STORAGE_BIT /*flags*/);

        glBindBuffer(GL_ARRAY_BUFFER, m_vbos[INDICES]);
        glBufferStorage(GL_ARRAY_BUFFER, sizeof(GLuint) * initEl.size(), &initEl[0], GL_DYNAMIC_STORAGE_BIT /*flags*/);

        glBindBuffer(GL_ARRAY_BUFFER, m_vbos[TEXCOORDS]);
        glBufferStorage(GL_ARRAY_BUFFER, sizeof(GLfloat) * initTc.size(), &initTc[0], GL_DYNAMIC_STORAGE_BIT /*flags*/);

        m_numElements = initEl.size();

        /* Set up VAO */
        glCreateVertexArrays(1, &m_vao);

        glEnableVertexArrayAttrib(m_vao, 0 /*index*/);
        glEnableVertexArrayAttrib(m_vao, 1 /*index*/);
        glEnableVertexArrayAttrib(m_vao, 2 /*index*/);

        glVertexArrayElementBuffer(m_vao, m_vbos[INDICES]);

        /* Separate attribute format */
        glVertexArrayAttribFormat(m_vao, 0 /*index*/, 4 /*size*/, GL_FLOAT, GL_FALSE, 0 /*relativeoffset*/);
        glVertexArrayAttribFormat(m_vao, 1 /*index*/, 4 /*size*/, GL_FLOAT, GL_FALSE, 0 /*relativeoffset*/);
        glVertexArrayAttribFormat(m_vao, 2 /*index*/, 2 /*size*/, GL_FLOAT, GL_FALSE, 0 /*relativeoffset*/);

        glVertexArrayAttribBinding(m_vao, 0 /*index*/, 0 /*bindingindex*/);
        glVertexArrayAttribBinding(m_vao, 1 /*index*/, 1 /*bindingindex*/);
        glVertexArrayAttribBinding(m_vao, 2 /*index*/, 2 /*bindingindex*/);

        glVertexArrayVertexBuffer(m_vao, 0 /*bindingindex*/, m_vbos[POSITIONS_0], 0 /*offset*/, sizeof(glm::vec4) /*stride*/);
        glVertexArrayVertexBuffer(m_vao, 1 /*bindingindex*/, m_vbos[NORMALS], 0 /*offset*/, sizeof(glm::vec4) /*stride*/);
        glVertexArrayVertexBuffer(m_vao, 2 /*bindingindex*/, m_vbos[TEXCOORDS], 0 /*offset*/, sizeof(glm::vec2) /*stride*/);

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
        m_computeClothNormalsShader->bind();
        glDispatchCompute((GLuint)m_particlesSize.x / 10, (GLuint)m_particlesSize.y / 10, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }

    Cloth::~Cloth()
    {
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

    void Cloth::reset()
    {
        m_readBuf = 0;

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_vbos[POSITIONS_0]);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLfloat) * 4 * (GLsizeiptr)m_particlesSize.x * (GLsizeiptr)m_particlesSize.y, m_initPositions);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_vbos[POSITIONS_1]);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLfloat) * 4 * (GLsizeiptr)m_particlesSize.x * (GLsizeiptr)m_particlesSize.y, NULL);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_vbos[VELOCITIES_0]);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLfloat) * 4 * (GLsizeiptr)m_particlesSize.x * (GLsizeiptr)m_particlesSize.y, m_initVelocities);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_vbos[VELOCITIES_1]);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLfloat) * 4 * (GLsizeiptr)m_particlesSize.x * (GLsizeiptr)m_particlesSize.y, NULL);

        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);

        m_computeClothNormalsShader->bind();
        glDispatchCompute((GLuint)m_particlesSize.x / 10, (GLuint)m_particlesSize.y / 10, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }

    void Cloth::setDiffuseTexture(const std::filesystem::path & filepath)
    {
        //Texture * diff = CoreAssetManager::genTexture2D(filename);
        //diff->setTypeName(std::string("texture_diffuse"));

        //m_textures[0] = diff;
    }

    void Cloth::setSpecularTexture(const std::filesystem::path & filepath)
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
        if (simulate)
        {
            m_computeClothShader->bind();

            m_computeClothShader->setUniform("pins", 5, &m_pins[0]);
            m_computeClothShader->setUniform("Gravity", gravity);
            m_computeClothShader->setUniform("ParticleMass", particleMass);
            m_computeClothShader->setUniform("ParticleInvMass", 1.0f / particleMass);
            m_computeClothShader->setUniform("RestLengthHoriz", m_xRestLen);
            m_computeClothShader->setUniform("RestLengthVert", m_yRestLen);
            m_computeClothShader->setUniform("RestLengthDiag", m_diagRestLen);
            m_computeClothShader->setUniform("SpringK", springK);
            m_computeClothShader->setUniform("DeltaT", deltaTime);
            m_computeClothShader->setUniform("DampingConst", damping);
            m_computeClothShader->setUniform("shouldSelfCollide", shouldSelfCollide);

//            m_compute_cloth_shader->setUniformMatrix4fv("model", m_world_transform);

            for (int i = 0; i < 50; ++i)
            {
                glDispatchCompute((GLuint)m_particlesSize.x / 10, (GLuint)m_particlesSize.y / 10, 1);
                glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

                /* Swap buffers */
                m_readBuf = 1 - m_readBuf;
                glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_vbos[m_readBuf]);
                glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_vbos[1 - m_readBuf]);
                glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_vbos[m_readBuf + 2]);
                glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, m_vbos[1 - m_readBuf + 2]);
            }

            m_computeClothNormalsShader->bind();
            glDispatchCompute((GLuint)m_particlesSize.x / 10, (GLuint)m_particlesSize.y / 10, 1);
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

        glBindVertexArray(m_vao);
        glDrawElements(GL_TRIANGLE_STRIP, m_numElements, GL_UNSIGNED_INT, NULL);
    }
}
