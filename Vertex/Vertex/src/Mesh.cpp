#include "Mesh.h"

Mesh::Mesh() : draw_mode(GL_TRIANGLES), indices_count (0)
{
    glCreateVertexArrays(1, &vao_id);
    glCreateBuffers     (sizeof(vbo_ids) / sizeof(GLuint), vbo_ids);
}


Mesh::~Mesh()
{
    if (vao_id != 0)
    {
        glDeleteVertexArrays(1, &vao_id);
    }

    if (vbo_ids[0] != 0)
    {
        glDeleteBuffers(sizeof(vbo_ids) / sizeof(GLuint), vbo_ids);
    }
}

void Mesh::render(Shader * shader)
{
    for (GLuint i = 0; i < textures.size(); ++i)
    {
        textures[i]->bind(i);
    }

    shader->setUniform1f ("material.shininess", material.shininess);
    shader->setUniform3fv("material.diffuseColor", material.diffuse_color);
    shader->updateUBOs();

    glBindVertexArray(vao_id);
    
    glDrawElements(draw_mode, indices_count, GL_UNSIGNED_SHORT, nullptr);
}

void Mesh::setBuffers(VEBuffers & buffers)
{
    /* Set up buffer objects */
    glNamedBufferStorage(vbo_ids[POSITION], buffers.positions.size() * sizeof(buffers.positions[0]), buffers.positions.data(), 0 /*flags*/);
    glNamedBufferStorage(vbo_ids[NORMAL],   buffers.normals.size()   * sizeof(buffers.normals[0]),   buffers.normals.data(),   0 /*flags*/);
    glNamedBufferStorage(vbo_ids[TEXCOORD], buffers.texcoords.size() * sizeof(buffers.texcoords[0]), buffers.texcoords.data(), 0 /*flags*/);
    glNamedBufferStorage(vbo_ids[INDEX],    buffers.indices.size()   * sizeof(buffers.indices[0]),   buffers.indices.data(),   0 /*flags*/);

    /* Set up VAO */
    glEnableVertexArrayAttrib(vao_id, 0 /*index*/);
    glEnableVertexArrayAttrib(vao_id, 1 /*index*/);
    glEnableVertexArrayAttrib(vao_id, 2 /*index*/);

    glVertexArrayElementBuffer(vao_id, vbo_ids[INDEX]);
    
    /* Separate attribute format */
    glVertexArrayAttribFormat(vao_id, 0 /*index*/, 3 /*size*/, GL_FLOAT, GL_FALSE, 0 /*relativeoffset*/);
    glVertexArrayAttribFormat(vao_id, 1 /*index*/, 3 /*size*/, GL_FLOAT, GL_FALSE, 0 /*relativeoffset*/);
    glVertexArrayAttribFormat(vao_id, 2 /*index*/, 2 /*size*/, GL_FLOAT, GL_FALSE, 0 /*relativeoffset*/);

    glVertexArrayAttribBinding(vao_id, 0 /*index*/, 0 /*bindingindex*/);
    glVertexArrayAttribBinding(vao_id, 1 /*index*/, 1 /*bindingindex*/);
    glVertexArrayAttribBinding(vao_id, 2 /*index*/, 2 /*bindingindex*/);

    glVertexArrayVertexBuffer(vao_id, 0 /*bindingindex*/, vbo_ids[POSITION], 0 /*offset*/, sizeof(glm::vec3) /*stride*/);
    glVertexArrayVertexBuffer(vao_id, 1 /*bindingindex*/, vbo_ids[NORMAL],   0 /*offset*/, sizeof(glm::vec3) /*stride*/);
    glVertexArrayVertexBuffer(vao_id, 2 /*bindingindex*/, vbo_ids[TEXCOORD], 0 /*offset*/, sizeof(glm::vec2) /*stride*/);
}
