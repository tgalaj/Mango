#pragma once

#include <glm/gtc/matrix_transform.hpp>

#include "core_components/CameraComponent.h"
#include "core_engine/CoreAssetManager.h"
#include "Texture.h"

namespace Vertex
{
    class Skybox
    {
        friend class Renderer;

    private:
        Skybox();
        ~Skybox();

        void render(Shader * shader, CameraComponent * cam);
        void loadImages(std::string * filenames);

        void setSize(float _size)
        {
            m_size = _size;
            m_world = glm::scale(glm::mat4(1.0f), glm::vec3(m_size));
        }

        glm::mat4 m_world;

        GLuint m_vao_id;
        GLuint m_vbo_ids[2];
        GLuint m_indices_count;

        Texture * m_cube_map;

        float m_size;
    };
}

