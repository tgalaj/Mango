#include "Cloth.h"
#include "ShaderManager.h"

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <math.h>
#include <vector>

Cloth::Cloth       (int particles_x, int particles_y, float cloth_size_x, float cloth_size_y)
    : particles_dim(particles_x, particles_y),
      cloth_size   (cloth_size_x, cloth_size_y),
      PRIM_RESTART (0xffffff),
      read_buf     (0)
{
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
    transf = glm::rotate(transf, glm::radians(-80.0f), glm::vec3(1, 0, 0));
    transf = glm::translate(transf, glm::vec3(0, -cloth_size.y, 0));

    glm::vec4 p(0.0f, 0.0f, 0.0f, 1.0f);

    float dx = cloth_size.x / (particles_dim.x - 1),
          dy = cloth_size.y / (particles_dim.y - 1),
          ds = 1.0f         / (particles_dim.x - 1),
          dt = 1.0f         / (particles_dim.y - 1);

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

    compute_cloth_shader         = ShaderManager::getShader("ve_compute_cloth");
    compute_cloth_normals_shader = ShaderManager::getShader("ve_compute_cloth_normals");
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

void Cloth::compute()
{
    if (simulate)
    {
        compute_cloth_shader->apply();

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

    glBindVertexArray(vao_id);
    glDrawElements(GL_TRIANGLE_STRIP, num_elements, GL_UNSIGNED_INT, 0);
}
