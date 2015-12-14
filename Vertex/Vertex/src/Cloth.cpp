#include "Cloth.h"
#include "ShaderManager.h"
#include "CoreAssetManager.h"

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <math.h>

Cloth::Cloth           (int particles_x, int particles_y, float cloth_size_x, float cloth_size_y)
    : particles_dim    (particles_x, particles_y),
      cloth_size       (cloth_size_x, cloth_size_y),
      PRIM_RESTART     (0xffffff),
      read_buf         (0),
      simulate         (true),
      gravity(glm::vec3(0.0f, -9.81f, 0.0f)),
      particle_mass    (0.1f),
      spring_k         (2000.0f),
      delta_t          (0.000005f),
      damping          (0.1f)
{
    for (auto & pin : pins)
    {
        pin = true;
    }

    if (particles_dim.x * particles_dim.y > 18000000)
    {
        particles_dim.x = 4242;
        particles_dim.y = 4242;
    }

    if (particles_dim.x * particles_dim.y < 1000)
    {
        particles_dim.x = 32;
        particles_dim.y = 32;
    }

    init_positions  = new GLfloat[particles_dim.x * particles_dim.y * 4];
    init_velocities = new GLfloat[particles_dim.x * particles_dim.y * 4];

    std::vector<GLfloat> init_tc;
    std::vector<GLuint>  init_el;

    memset(init_positions,  0, sizeof(GLfloat) * 4 * particles_dim.x * particles_dim.y);
    memset(init_velocities, 0, sizeof(GLfloat) * 4 * particles_dim.x * particles_dim.y);

    glm::mat4 transf = glm::translate(glm::mat4(1.0), glm::vec3(0, cloth_size.y, 0));
    transf = glm::rotate(transf, glm::radians(80.0f), glm::vec3(1, 0, 0));
    transf = glm::translate(transf, glm::vec3(0, -cloth_size.y, 0));

    glm::vec4 p(0.0f, 0.0f, 0.0f, 1.0f);

    float dx = cloth_size.x / (particles_dim.x - 1),
          dy = cloth_size.y / (particles_dim.y - 1),
          ds = 1.0f         / (particles_dim.x - 1),
          dt = 1.0f         / (particles_dim.y - 1);

    rest_len_x    = dx;
    rest_len_y    = dy;
    rest_len_diag = glm::sqrt(dx * dx + dy * dy);

    int counter = 0;
    for (int i = 0; i < particles_dim.y; ++i)
    {
        for (int j = 0; j < particles_dim.x; ++j)
        {
            p.x = dx * j;
            p.y = dy * i;
            p.z = 0.0f;
            
            p = transf * p;

            init_positions[counter++] = p.x;
            init_positions[counter++] = p.y;
            init_positions[counter++] = p.z;
            init_positions[counter++] = p.w;

            init_tc.push_back(ds * j);
            init_tc.push_back(dt * i);
        }
    }

    for (int row = 0; row < particles_dim.y - 1; ++row)
    {
        for (int col = 0; col < particles_dim.x; ++col)
        {
            init_el.push_back((row + 1) * particles_dim.x + col);
            init_el.push_back( row      * particles_dim.x + col);
        }
        init_el.push_back(PRIM_RESTART);
    }

    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex(PRIM_RESTART);

    /* VBOs */
    GLuint no_parts = particles_dim.x * particles_dim.y;

    glGenBuffers(7, vbo_ids);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vbo_ids[POSITIONS_0]);
    glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(GLfloat) * 4 * no_parts, init_positions, GL_DYNAMIC_STORAGE_BIT /*flags*/);
    
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, vbo_ids[POSITIONS_1]);
    glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(GLfloat) * 4 * no_parts, NULL, GL_DYNAMIC_STORAGE_BIT /*flags*/);
   
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, vbo_ids[VELOCITIES_0]);
    glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(GLfloat) * 4 * no_parts, init_velocities, GL_DYNAMIC_STORAGE_BIT /*flags*/);
   
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, vbo_ids[VELOCITIES_1]);
    glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(GLfloat) * 4 * no_parts, NULL, GL_DYNAMIC_STORAGE_BIT /*flags*/);
   
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, vbo_ids[NORMALS]);
    glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(GLfloat) * 4 * no_parts, NULL, GL_DYNAMIC_STORAGE_BIT /*flags*/);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[INDICES]);
    glBufferStorage(GL_ARRAY_BUFFER, sizeof(GLuint) * init_el.size(), &init_el[0], GL_DYNAMIC_STORAGE_BIT /*flags*/);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[TEXCOORDS]);
    glBufferStorage(GL_ARRAY_BUFFER, sizeof(GLfloat) * init_tc.size(), &init_tc[0], GL_DYNAMIC_STORAGE_BIT /*flags*/);

    num_elements = init_el.size();

    /* Set up VAO */
    glCreateVertexArrays(1, &vao_id);
    
    glEnableVertexArrayAttrib(vao_id, 0 /*index*/);
    glEnableVertexArrayAttrib(vao_id, 1 /*index*/);
    glEnableVertexArrayAttrib(vao_id, 2 /*index*/);
    
    glVertexArrayElementBuffer(vao_id, vbo_ids[INDICES]);
    
    /* Separate attribute format */
    glVertexArrayAttribFormat(vao_id, 0 /*index*/, 4 /*size*/, GL_FLOAT, GL_FALSE, 0 /*relativeoffset*/);
    glVertexArrayAttribFormat(vao_id, 1 /*index*/, 4 /*size*/, GL_FLOAT, GL_FALSE, 0 /*relativeoffset*/);
    glVertexArrayAttribFormat(vao_id, 2 /*index*/, 2 /*size*/, GL_FLOAT, GL_FALSE, 0 /*relativeoffset*/);
    
    glVertexArrayAttribBinding(vao_id, 0 /*index*/, 0 /*bindingindex*/);
    glVertexArrayAttribBinding(vao_id, 1 /*index*/, 1 /*bindingindex*/);
    glVertexArrayAttribBinding(vao_id, 2 /*index*/, 2 /*bindingindex*/);
    
    glVertexArrayVertexBuffer(vao_id, 0 /*bindingindex*/, vbo_ids[POSITIONS_0], 0 /*offset*/, sizeof(glm::vec4) /*stride*/);
    glVertexArrayVertexBuffer(vao_id, 1 /*bindingindex*/, vbo_ids[NORMALS],     0 /*offset*/, sizeof(glm::vec4) /*stride*/);
    glVertexArrayVertexBuffer(vao_id, 2 /*bindingindex*/, vbo_ids[TEXCOORDS],   0 /*offset*/, sizeof(glm::vec2) /*stride*/);

    /* Get shaders */
    compute_cloth_shader         = ShaderManager::getShader("ve_compute_cloth");
    compute_cloth_normals_shader = ShaderManager::getShader("ve_compute_cloth_normals");

    /* Set up textures and material */
    Texture * diff = CoreAssetManager::createTexture2D("res/texture/white_4x4.jpg");
    diff->setTypeName(std::string("texture_diffuse"));

    Texture * spec = CoreAssetManager::createTexture2D("res/texture/spec_default.jpg");
    spec->setTypeName(std::string("texture_specular"));

    textures.push_back(diff);
    textures.push_back(spec);

    material.diffuse_color = glm::vec3(0.8f, 0.8f, 0.8f);
    material.shininess     = 2.0f;

    /* Compute normals */
    compute_cloth_normals_shader->apply();
    glDispatchCompute(particles_dim.x / 10, particles_dim.y / 10, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

Cloth::~Cloth()
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

void Cloth::reset()
{
    read_buf = 0;

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vbo_ids[POSITIONS_0]);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER,  0, sizeof(GLfloat) * 4 * particles_dim.x * particles_dim.y, init_positions);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, vbo_ids[POSITIONS_1]);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLfloat) * 4 * particles_dim.x * particles_dim.y, NULL);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, vbo_ids[VELOCITIES_0]);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER,  0, sizeof(GLfloat) * 4 * particles_dim.x * particles_dim.y, init_velocities);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, vbo_ids[VELOCITIES_1]);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLfloat) * 4 * particles_dim.x * particles_dim.y, NULL);

    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_BUFFER_UPDATE_BARRIER_BIT);

    compute_cloth_normals_shader->apply();
    glDispatchCompute(particles_dim.x / 10, particles_dim.y / 10, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
}

void Cloth::setDiffuseTexture(const std::string & filename)
{
    Texture * diff = CoreAssetManager::createTexture2D(filename);
    diff->setTypeName(std::string("texture_diffuse"));

    textures[0] = diff;
}

void Cloth::setSpecularTexture(const std::string & filename)
{
    Texture * spec = CoreAssetManager::createTexture2D(filename);
    spec->setTypeName(std::string("texture_specular"));

    textures[1] = spec;
}

void Cloth::setColor(glm::vec3 & color)
{
    material.diffuse_color = color;
}

void Cloth::setShininess(float shinines)
{
    material.shininess = shinines;
}

void Cloth::setPin(int idx, bool isActive)
{
    if (idx < 0)
    {
        idx = 0;
    }

    if (idx >= sizeof(pins) / sizeof(int))
    {
        idx = sizeof(pins) / sizeof(int) - 1;
    }

    pins[idx] = isActive;
}

void Cloth::compute()
{
    if (simulate)
    {
        compute_cloth_shader->apply();
        
        compute_cloth_shader->setUniform1iv("pins", 5,        &pins[0]);
        compute_cloth_shader->setUniform3fv("Gravity",         gravity);
        compute_cloth_shader->setUniform1f ("ParticleMass",    particle_mass);
        compute_cloth_shader->setUniform1f ("ParticleInvMass", 1.0f / particle_mass);
        compute_cloth_shader->setUniform1f ("RestLengthHoriz", rest_len_x);
        compute_cloth_shader->setUniform1f ("RestLengthVert",  rest_len_y);
        compute_cloth_shader->setUniform1f ("RestLengthDiag",  rest_len_diag);
        compute_cloth_shader->setUniform1f ("SpringK",         spring_k);
        compute_cloth_shader->setUniform1f ("DeltaT",          delta_t);
        compute_cloth_shader->setUniform1f ("DampingConst",    damping);

        for(int i = 0; i < 1000; ++i)
        {
            glDispatchCompute(particles_dim.x / 10, particles_dim.y / 10, 1);
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

            /* Swap buffers */
            read_buf = 1 - read_buf;
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vbo_ids[read_buf]);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, vbo_ids[1 - read_buf]);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, vbo_ids[read_buf + 2]);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, vbo_ids[1 - read_buf + 2]);
        }

        compute_cloth_normals_shader->apply();
        glDispatchCompute(particles_dim.x / 10, particles_dim.y / 10, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }
}

void Cloth::render(Shader * shader)
{
    shader->setUniformMatrix3fv("normalMatrix", normalMatrix);
    shader->setUniformMatrix4fv("world", worldTransform);

    for (GLuint i = 0; i < textures.size(); ++i)
    {
        textures[i]->bind(i);
    }

    shader->setUniform1f ("material.shininess",    material.shininess);
    shader->setUniform3fv("material.diffuseColor", material.diffuse_color);

    glBindVertexArray(vao_id);
    glDrawElements(GL_TRIANGLE_STRIP, num_elements, GL_UNSIGNED_INT, NULL);
}
