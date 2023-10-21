#include "mgpch.h"

#include "Skybox.h"
#include "Mesh.h"
#include "Utilities/GeomPrimitive.h"

namespace mango
{
    Skybox::Skybox(const std::string & skyboxDirectory,
                   const std::string & leftFace,
                   const std::string & rightFace,
                   const std::string & upFace,
                   const std::string & downFace,
                   const std::string & frontFace,
                   const std::string & backFace) : m_world(glm::mat4(1.0f))
    {
        /* Create cubemap texture object */
        std::filesystem::path filenames[6] = {
            skyboxDirectory + "/" + leftFace,
            skyboxDirectory + "/" + rightFace,
            skyboxDirectory + "/" + upFace,
            skyboxDirectory + "/" + downFace,
            skyboxDirectory + "/" + frontFace,
            skyboxDirectory + "/" + backFace
        };
        m_cubeMapTexture = CoreAssetManager::createCubeMapTexture(filenames, true);

        /* Create skybox shader object */
        m_skyboxShader = CoreAssetManager::createShader("Skybox", "Skybox.vert", "Skybox.frag");
        m_skyboxShader->link();

        /* Create buffer objects */
        glCreateVertexArrays(1, &m_vao);
        glCreateBuffers(1, &m_vbo);

        std::vector<float> skyboxPositions = {
            // positions          
            -1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f, -1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,

            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,

            -1.0f,  1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,

            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f,
             1.0f, -1.0f,  1.0f
        };

        /* Set up buffer objects */
        glNamedBufferStorage(m_vbo, skyboxPositions.size() * sizeof(skyboxPositions[0]), skyboxPositions.data(), 0 /*flags*/);

        /* Set up VAO */
        glEnableVertexArrayAttrib (m_vao, 0 /*index*/);

        /* Separate attribute format */
        glVertexArrayAttribFormat (m_vao, 0 /*index*/, 3 /*size*/, GL_FLOAT, GL_FALSE, 0 /*relativeoffset*/);
        glVertexArrayAttribBinding(m_vao, 0 /*index*/, 0 /*bindingindex*/);
        glVertexArrayVertexBuffer (m_vao, 0 /*bindingindex*/, m_vbo, 0 /*offset*/, sizeof(glm::vec3) /*stride*/);
    }


    Skybox::~Skybox()
    {
        if (m_vao != 0)
        {
            glDeleteVertexArrays(1, &m_vao);
        }

        if (m_vbo != 0)
        {
            glDeleteBuffers(1, &m_vbo);
        }
    }

    void Skybox::render(const glm::mat4 & projection, const glm::mat4 & view)
    {
        m_skyboxShader->bind();
        m_skyboxShader->setUniform("view_projection", projection * glm::mat4(glm::mat3(view)));

        m_cubeMapTexture->bind();
        glBindVertexArray(m_vao);

        glDepthFunc(GL_LEQUAL);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthFunc(GL_LESS);
    }

    void Skybox::bindSkyboxTexture(GLuint unit)
    {
        m_cubeMapTexture->bind(unit);
    }
}