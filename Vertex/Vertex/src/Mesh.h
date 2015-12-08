#pragma once

#include <GL/glew.h>
#include <glm\glm.hpp>
#include <vector>

#include "Material.h"
#include "Texture.h"
#include "Shader.h"

struct VEBuffers
{
    std::vector<glm::vec3> positions, normals;
    std::vector<glm::vec2> texcoords;
    std::vector<GLushort>  indices;
};

class Mesh
{
public:
    Mesh();
    ~Mesh();

    Mesh & operator=(const Mesh &) = delete;

private:
    friend class Model;
    friend class CoreAssetManager;
    friend class Renderer;

    Mesh * clone() const
    {
        return new Mesh(*this);
    }

    void render(Shader * shader);
    void setBuffers(VEBuffers & buffers);

    std::vector<Texture *> textures;
    Material material;

    GLuint vao_id;
    GLuint vbo_ids[4];
    GLuint indices_count;
    GLenum draw_mode;

    enum { POSITION, NORMAL, TEXCOORD, INDEX };
};

