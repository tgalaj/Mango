#include "Skybox.h"
#include "Mesh.h"
#include "GeomPrimitive.h"

Skybox::Skybox()
    : size(100.0f)
{
    glCreateVertexArrays(1, &vao_id);
    glCreateBuffers     (sizeof(vbo_ids) / sizeof(GLuint), vbo_ids);

    VEBuffers buffers;
    GeomPrimitive::genCube(buffers, 1.0f);
    indices_count = buffers.indices.size();

    /* Set up buffer objects */
    glNamedBufferStorage(vbo_ids[0], buffers.positions.size() * sizeof(buffers.positions[0]), buffers.positions.data(), 0 /*flags*/);
    glNamedBufferStorage(vbo_ids[1], buffers.indices.size()   * sizeof(buffers.indices[0]),   buffers.indices.data(),   0 /*flags*/);

    /* Set up VAO */
    glEnableVertexArrayAttrib(vao_id, 0 /*index*/);

    glVertexArrayElementBuffer(vao_id, vbo_ids[1]);

    /* Separate attribute format */
    glVertexArrayAttribFormat(vao_id, 0 /*index*/, 3 /*size*/, GL_FLOAT, GL_FALSE, 0 /*relativeoffset*/);

    glVertexArrayAttribBinding(vao_id, 0 /*index*/, 0 /*bindingindex*/);

    glVertexArrayVertexBuffer(vao_id, 0 /*bindingindex*/, vbo_ids[0], 0 /*offset*/, sizeof(glm::vec3) /*stride*/);

    world = glm::scale(glm::mat4(1.0f), glm::vec3(size));
}


Skybox::~Skybox()
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

void Skybox::render(Shader * shader, Camera * cam)
{
    cubeMap->bind(0);
    glBindVertexArray(vao_id);
    glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_SHORT, nullptr);
}

void Skybox::loadImages(std::string * filenames)
{
    cubeMap = CoreAssetManager::createCubeMapTexture(filenames, 0);
}
