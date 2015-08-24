#include "Model.h"
#include "GeomPrimitive.h"

Model::Model() : draw_mode(GL_TRIANGLES)
{
    glGenVertexArrays(1, &vao_id);
    glGenBuffers     (sizeof(vbo_ids) / sizeof(GLuint), vbo_ids);
}

Model::Model(std::string filename) : Model()
{
    //TODO: model loading from file - ASSIMP
}

Model::~Model()
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

void Model::render()
{
    glBindVertexArray(vao_id);

    glDrawElements(draw_mode, indices_count, GL_UNSIGNED_BYTE, nullptr);
    
    glBindVertexArray(0);
}

void Model::genCube(float radius)
{
    VEBuffers buffers;

    GeomPrimitive::genCube(buffers, radius);
    setBuffers(buffers);

    indices_count = buffers.indices.size();
}

void Model::setBuffers(VEBuffers &buffers)
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
