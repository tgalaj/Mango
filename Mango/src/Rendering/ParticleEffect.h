#pragma once

#include <glad/glad.h>

#include "CoreComponents/CameraComponent.h"
#include "Shader.h"

namespace mango
{
    class ParticleEffect
    {
    public:
        friend class Renderer;

        ParticleEffect(GLuint _max_particles);
        ~ParticleEffect();

        glm::vec4 m_color;
        bool m_simulate;

        void reset();

    private:
        GLfloat * m_init_positions;
        GLfloat * m_init_velocities;

        Shader * m_shader;

        GLuint m_vbo_ids[2];
        GLuint m_vao_id;

        GLuint m_max_particles;

        enum { POSITIONS, VELOCITIES };

        void render(CameraComponent * cam);
    };
}
