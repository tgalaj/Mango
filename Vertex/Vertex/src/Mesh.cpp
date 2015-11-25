#include "Mesh.h"

Mesh::Mesh() : draw_mode(GL_TRIANGLES), indices_count (0)
{
    glGenVertexArrays(1, &vao_id);
    glGenBuffers     (sizeof(vbo_ids) / sizeof(GLuint), vbo_ids);
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
        shader->setUniform1i("material." + textures[i]->getTypeName(), i);
        textures[i]->bind(GL_TEXTURE0 + i);
    }

    shader->setUniform1f ("material.shininess", material.shininess);
    shader->setUniform3fv("material.diffuseColor", material.diffuse_color);

    glBindVertexArray(vao_id);

    glDrawElements(draw_mode, indices_count, GL_UNSIGNED_SHORT, nullptr);
}

void Mesh::setBuffers(VEBuffers & buffers)
{
    /* Set up buffer objects */
    glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[POSITION]);
    glBufferStorage(GL_ARRAY_BUFFER, buffers.positions.size() * sizeof(buffers.positions[0]), buffers.positions.data(), 0 /*flags*/);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[NORMAL]);
    glBufferStorage(GL_ARRAY_BUFFER, buffers.normals.size() * sizeof(buffers.normals[0]), buffers.normals.data(), 0 /*flags*/);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[TEXCOORD]);
    glBufferStorage(GL_ARRAY_BUFFER, buffers.texcoords.size() * sizeof(buffers.texcoords[0]), buffers.texcoords.data(), 0 /*flags*/);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_ids[INDEX]);
    glBufferStorage(GL_ELEMENT_ARRAY_BUFFER, buffers.indices.size() * sizeof(buffers.indices[0]), buffers.indices.data(), 0 /*flags*/);

    /* Set up VAO */
    glBindVertexArray(vao_id);
    
    glEnableVertexAttribArray(0 /*index*/);
    glEnableVertexAttribArray(1 /*index*/);
    glEnableVertexAttribArray(2 /*index*/);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_ids[INDEX]);

    /* Separate attribute format */
    glVertexAttribFormat(0 /*index*/, 3 /*size*/, GL_FLOAT, GL_FALSE, 0 /*relativeoffset*/);
    glVertexAttribFormat(1 /*index*/, 3 /*size*/, GL_FLOAT, GL_FALSE, 0 /*relativeoffset*/);
    glVertexAttribFormat(2 /*index*/, 2 /*size*/, GL_FLOAT, GL_FALSE, 0 /*relativeoffset*/);

    glVertexAttribBinding(0 /*index*/, 0 /*bindingindex*/);
    glVertexAttribBinding(1 /*index*/, 1 /*bindingindex*/);
    glVertexAttribBinding(2 /*index*/, 2 /*bindingindex*/);

    glBindVertexBuffer(0 /*bindingindex*/, vbo_ids[POSITION], 0 /*offset*/, sizeof(glm::vec3) /*stride*/);
    glBindVertexBuffer(1 /*bindingindex*/, vbo_ids[NORMAL],   0 /*offset*/, sizeof(glm::vec3) /*stride*/);
    glBindVertexBuffer(2 /*bindingindex*/, vbo_ids[TEXCOORD], 0 /*offset*/, sizeof(glm::vec2) /*stride*/);

    glBindVertexArray(0);
}
