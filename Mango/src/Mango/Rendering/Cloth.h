#pragma once

#include <glad/glad.h>
#include <string>
#include <vector>

#include "Material.h"
#include "Shader.h"
#include "Texture.h"

namespace mango
{
    class Cloth
    {
    public:
        friend class Renderer;

        Cloth(int particles_x, int particles_y, float cloth_size_x, float cloth_size_y);
        ~Cloth();

        bool simulate;
        bool shouldSelfCollide;

        glm::vec3 gravity;
        float     particleMass;
        float     springK;
        float     deltaTime;
        float     damping;

        void reset();
        void setDiffuseTexture (const std::filesystem::path & filepath);
        void setSpecularTexture(const std::filesystem::path & filepath);

        void setColor(glm::vec3 & color);
        void setShininess(float shinines);

        void setPin(int idx, bool isActive);

    private:
        GLfloat * m_initPositions;
        GLfloat * m_initVelocities;

        Shader * m_computeClothShader;
        Shader * m_computeClothNormalsShader;

        std::vector<Texture *> m_textures;
        Material m_material;

        glm::vec2 m_particlesSize;
        glm::vec2 m_clothSize;

        float m_xRestLen;
        float m_yRestLen;
        float m_diagRestLen;

        GLuint m_vbos[7];
        GLuint m_vao;
        GLuint m_numElements;
        GLchar m_readBuf;

        const GLuint PRIM_RESTART;

        enum { POSITIONS_0, POSITIONS_1, VELOCITIES_0, VELOCITIES_1, NORMALS, INDICES, TEXCOORDS };

        int m_pins[5];

        void compute();
        void render(Shader * shader);
    };
}