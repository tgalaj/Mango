#include "framework/rendering/Skybox.h"
#include "framework/rendering/Mesh.h"
#include "framework/utilities/GeomPrimitive.h"

namespace Vertex
{
    Skybox::Skybox()
        : m_cube_map(nullptr), m_size(100.0f)
    {
        glCreateVertexArrays(1, &m_vao_id);
        glCreateBuffers(sizeof(m_vbo_ids) / sizeof(GLuint), m_vbo_ids);

        VertexBuffers buffers;
        GeomPrimitive::genCube(buffers, 1.0f);
        m_indices_count = buffers.m_indices.size();

        /* Set up buffer objects */
//        glNamedBufferStorage(m_vbo_ids[0], buffers.m_positions.size() * sizeof(buffers.m_positions[0]), buffers.m_positions.data(), 0 /*flags*/);
        glNamedBufferStorage(m_vbo_ids[1], buffers.m_indices.size() * sizeof(buffers.m_indices[0]), buffers.m_indices.data(), 0 /*flags*/);

        /* Set up VAO */
        glEnableVertexArrayAttrib(m_vao_id, 0 /*index*/);

        glVertexArrayElementBuffer(m_vao_id, m_vbo_ids[1]);

        /* Separate attribute format */
        glVertexArrayAttribFormat(m_vao_id, 0 /*index*/, 3 /*size*/, GL_FLOAT, GL_FALSE, 0 /*relativeoffset*/);

        glVertexArrayAttribBinding(m_vao_id, 0 /*index*/, 0 /*bindingindex*/);

        glVertexArrayVertexBuffer(m_vao_id, 0 /*bindingindex*/, m_vbo_ids[0], 0 /*offset*/, sizeof(glm::vec3) /*stride*/);

        m_world = glm::scale(glm::mat4(1.0f), glm::vec3(m_size));
    }


    Skybox::~Skybox()
    {
        if (m_vao_id != 0)
        {
            glDeleteVertexArrays(1, &m_vao_id);
        }

        if (m_vbo_ids[0] != 0)
        {
            glDeleteBuffers(sizeof(m_vbo_ids) / sizeof(GLuint), m_vbo_ids);
        }
    }

    void Skybox::render(Shader * shader, CameraComponent * cam)
    {
        m_cube_map->bind(0);
        glBindVertexArray(m_vao_id);
        glDrawElements(GL_TRIANGLES, m_indices_count, GL_UNSIGNED_SHORT, nullptr);
    }

    void Skybox::loadImages(std::string * filenames)
    {
        //m_cube_map = CoreAssetManager::genCubeMapTexture(filenames, 0);
    }
}
