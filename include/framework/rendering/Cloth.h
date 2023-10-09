#pragma once

#include <glad/glad.h>

#include "Shader.h"
#include "Texture.h"
#include "Material.h"

#include <string>
#include <vector>

namespace mango
{
    class Cloth
    {
    public:
        friend class Renderer;

        Cloth(int particles_x, int particles_y, float cloth_size_x, float cloth_size_y);
        ~Cloth();

        bool m_simulate;
        bool m_should_self_collide;

        glm::vec3 m_gravity;
        float m_particle_mass;
        float m_spring_k;
        float m_delta_t;
        float m_damping;

        void reset();
        void setDiffuseTexture(const std::string & filename);
        void setSpecularTexture(const std::string & filename);

        void setColor(glm::vec3 & color);
        void setShininess(float shinines);

        void setPin(int idx, bool isActive);

    private:
        GLfloat * m_init_positions;
        GLfloat * m_init_velocities;

        Shader * m_compute_cloth_shader;
        Shader * m_compute_cloth_normals_shader;

        std::vector<Texture *> m_textures;
        Material m_material;

        glm::vec2 m_particles_dim;
        glm::vec2 m_cloth_size;

        float m_rest_len_x;
        float m_rest_len_y;
        float m_rest_len_diag;

        GLuint m_vbo_ids[7];
        GLuint m_vao_id;
        GLuint m_num_elements;
        GLchar m_read_buf;

        const GLuint PRIM_RESTART;

        enum { POSITIONS_0, POSITIONS_1, VELOCITIES_0, VELOCITIES_1, NORMALS, INDICES, TEXCOORDS };

        int m_pins[5];

        void compute();
        void render(Shader * shader);
    };
}