#include "mgpch.h"

#include "Skybox.h"
#include "Mesh.h"

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
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("Skybox::Skybox");

        /* Create cubemap texture object */
        std::string filenames[6] = {
            skyboxDirectory + "/" + leftFace,
            skyboxDirectory + "/" + rightFace,
            skyboxDirectory + "/" + upFace,
            skyboxDirectory + "/" + downFace,
            skyboxDirectory + "/" + frontFace,
            skyboxDirectory + "/" + backFace
        };
        m_cubeMapTexture = AssetManager::createCubeMapTexture(filenames, true);

        /* Create skybox shader object */
        m_skyboxShader = AssetManager::createShader("Skybox", "Skybox.vert", "Skybox.frag");
        m_skyboxShader->link();

        m_skyboxMesh = createRef<Mesh>();
        m_skyboxMesh->genCubeMap(2.0f);
    }

    void Skybox::render(const glm::mat4 & projection, const glm::mat4 & view)
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("Skybox::render");

        m_skyboxShader->bind();
        m_skyboxShader->setUniform("view_projection", projection * glm::mat4(glm::mat3(view)));

        m_cubeMapTexture->bind(0);
        m_skyboxMesh->bind();

        glDepthFunc(GL_LEQUAL);
        m_skyboxMesh->render();
        glDepthFunc(GL_LESS);
    }

    void Skybox::bindSkyboxTexture(GLuint unit)
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("Skybox::bindSkyboxTexture");

        m_cubeMapTexture->bind(unit);
    }
}
