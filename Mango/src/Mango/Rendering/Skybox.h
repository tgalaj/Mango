#pragma once

#include <glm/mat4x4.hpp>

#include "Texture.h"
#include "Mango/Core/AssetManager.h"

namespace mango
{
    class Skybox
    {
    public:
        /*
         * Creates skybox object.
         * @param skyboxDirectory absolute or relative directory where skybox image faces are stored.
         * @param leftFace filename of the left  side [+x] of the cubemap
         * @param rightFace filename of the right side [-x] of the cubemap
         * @param upFace filename of the up    side [+y] of the cubemap
         * @param downFace filename of the down  side [-y] of the cubemap
         * @param frontFace filename of the front side [+z] of the cubemap
         * @param backFace filename of the back  side [-z] of the cubemap
         */
        Skybox(const std::string & skyboxDirectory,
               const std::string & leftFace,
               const std::string & rightFace,
               const std::string & upFace,
               const std::string & downFace,
               const std::string & frontFace,
               const std::string & backFace);
        ~Skybox() = default;

        void render(const glm::mat4 & projection, const glm::mat4 & view);
        void bindSkyboxTexture(GLuint unit = 0);

    private:
        glm::mat4 m_world;

        GLuint m_vao;
        GLuint m_vbo;

        ref<Shader>  m_skyboxShader;
        ref<Texture> m_cubeMapTexture;
        ref<Mesh>    m_skyboxMesh;
    };
}

