#pragma once

#include <glad/glad.h>

#include "Components/CameraComponent.h"
#include "Shader.h"

namespace mango
{
    class ParticleEffect
    {
    public:
        friend class Renderer;

        ParticleEffect(GLuint maxParticles);
        ~ParticleEffect();

        glm::vec4 color;
        bool      simulate;

        void reset();

    private:
        GLfloat * m_initPositions;
        GLfloat * m_initVelocities;

        Shader * m_shader;

        GLuint m_vbos[2];
        GLuint m_vao;

        GLuint m_maxParticles;

        enum { POSITIONS, VELOCITIES };

        void render(CameraComponent * cam);
    };
}
