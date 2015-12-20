#pragma once

#include <GL\glew.h>

#include "Camera.h"
#include "SceneNode.h"
#include "Shader.h"
#include "Texture.h"
#include "Material.h"

#include <string>
#include <vector>

class Cloth : public SceneNode
{
public:
    friend class Renderer;

    Cloth(int particles_x, int particles_y, float cloth_size_x, float cloth_size_y);
    ~Cloth();

    bool simulate;
    bool shouldSelfCollide;

    glm::vec3 gravity;
    float particle_mass;
    float spring_k;
    float delta_t;
    float damping;

    void reset();
    void setDiffuseTexture (const std::string & filename);
    void setSpecularTexture(const std::string & filename);

    void setColor    (glm::vec3 & color);
    void setShininess(float shinines);

    void setPin(int idx, bool isActive);
    
private:
    GLfloat * init_positions;
    GLfloat * init_velocities;

    Shader * compute_cloth_shader;
    Shader * compute_cloth_normals_shader;

    std::vector<Texture *> textures;
    Material material;

    glm::vec2 particles_dim;
    glm::vec2 cloth_size;

    float rest_len_x;
    float rest_len_y;
    float rest_len_diag;

    GLuint vbo_ids[7];
    GLuint vao_id;
    GLuint num_elements;
    GLchar read_buf;

    const GLuint PRIM_RESTART;

    enum { POSITIONS_0, POSITIONS_1, VELOCITIES_0, VELOCITIES_1, NORMALS, INDICES, TEXCOORDS };

    int pins[5];

    void compute();
    void render(Shader * shader);
};