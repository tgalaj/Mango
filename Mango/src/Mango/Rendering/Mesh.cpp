#include "mgpch.h"
#include "Mesh.h"

namespace mango
{
    MeshData::MeshData()
        : indicesCount(0),
          drawMode(GL_TRIANGLES)
    {
        glCreateVertexArrays(1, &vao);
        glCreateBuffers(sizeof(vbos) / sizeof(GLuint), vbos);
    }

    MeshData::~MeshData()
    {
        if (vao != 0)
        {
            glDeleteVertexArrays(1, &vao);
        }

        if (vbos[0] != 0)
        {
            glDeleteBuffers(sizeof(vbos) / sizeof(GLuint), vbos);
        }
    }

    Mesh::Mesh()
    {
    }

    Mesh::~Mesh()
    {
    }

    void Mesh::setBuffers(const VertexBuffers & buffers)
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("Mesh::setBuffers");

        m_meshData = std::make_shared<MeshData>();
        m_meshData->indicesCount = buffers.indices.size();

        /* Set up buffer objects */
        glNamedBufferStorage(m_meshData->vbos[VERTEX_DATA],  buffers.vertices.size()  * sizeof(buffers.vertices[0]),  buffers.vertices.data(),  0 /*flags*/);
        glNamedBufferStorage(m_meshData->vbos[INDEX],        buffers.indices.size()   * sizeof(buffers.indices[0]),   buffers.indices.data(),   0 /*flags*/);

        /* Set up VAO */
        glEnableVertexArrayAttrib(m_meshData->vao, 0 /*index*/);
        glEnableVertexArrayAttrib(m_meshData->vao, 1 /*index*/);
        glEnableVertexArrayAttrib(m_meshData->vao, 2 /*index*/);
        glEnableVertexArrayAttrib(m_meshData->vao, 3 /*index*/);

        glVertexArrayElementBuffer(m_meshData->vao, m_meshData->vbos[INDEX]);

        /* Separate attribute format */
        glVertexArrayAttribFormat(m_meshData->vao, 0 /*index*/, 3 /*size*/, GL_FLOAT, GL_FALSE, offsetof(VertexBuffers::Vertex, position) /*relativeoffset*/);
        glVertexArrayAttribFormat(m_meshData->vao, 1 /*index*/, 3 /*size*/, GL_FLOAT, GL_FALSE, offsetof(VertexBuffers::Vertex, normal)   /*relativeoffset*/);
        glVertexArrayAttribFormat(m_meshData->vao, 2 /*index*/, 3 /*size*/, GL_FLOAT, GL_FALSE, offsetof(VertexBuffers::Vertex, texcoord) /*relativeoffset*/);
        glVertexArrayAttribFormat(m_meshData->vao, 3 /*index*/, 3 /*size*/, GL_FLOAT, GL_FALSE, offsetof(VertexBuffers::Vertex, tangent)  /*relativeoffset*/);

        glVertexArrayAttribBinding(m_meshData->vao, 0 /*index*/, 0 /*bindingindex*/);
        glVertexArrayAttribBinding(m_meshData->vao, 1 /*index*/, 0 /*bindingindex*/);
        glVertexArrayAttribBinding(m_meshData->vao, 2 /*index*/, 0 /*bindingindex*/);
        glVertexArrayAttribBinding(m_meshData->vao, 3 /*index*/, 0 /*bindingindex*/);
        
        glVertexArrayVertexBuffer(m_meshData->vao, 0 /*bindingindex*/, m_meshData->vbos[VERTEX_DATA], 0 /*offset*/, sizeof(buffers.vertices[0]) /*stride*/);
    }

    void Mesh::render() const
    {        
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("Mesh::render");

        glBindVertexArray(m_meshData->vao);
        glDrawElements(m_meshData->drawMode, m_meshData->indicesCount, GL_UNSIGNED_INT, nullptr);
    }
}
