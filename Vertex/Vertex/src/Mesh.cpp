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

    glBindVertexArray(vao_id);

    glDrawElements(draw_mode, indices_count, GL_UNSIGNED_SHORT, nullptr);
}

void Mesh::setBuffers(VEBuffers & buffers)
{
    glBindVertexArray(vao_id);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[POSITION]);
    glBufferData(GL_ARRAY_BUFFER, buffers.positions.size() * sizeof(buffers.positions[0]), buffers.positions.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0 /*index*/);
    glVertexAttribPointer    (0 /*index*/, 3 /*size*/, GL_FLOAT, GL_FALSE, 0 /*stride*/, nullptr);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[NORMAL]);
    glBufferData(GL_ARRAY_BUFFER, buffers.normals.size() * sizeof(buffers.normals[0]), buffers.normals.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(1 /*index*/);
    glVertexAttribPointer    (1 /*index*/, 3 /*size*/, GL_FLOAT, GL_FALSE, 0 /*stride*/, nullptr);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_ids[TEXCOORD]);
    glBufferData(GL_ARRAY_BUFFER, buffers.texcoords.size() * sizeof(buffers.texcoords[0]), buffers.texcoords.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(2 /*index*/);
    glVertexAttribPointer    (2 /*index*/, 2 /*size*/, GL_FLOAT, GL_FALSE, 0 /*stride*/, nullptr);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_ids[INDEX]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, buffers.indices.size() * sizeof(buffers.indices[0]), buffers.indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
}
