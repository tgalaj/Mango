#pragma once

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include <string>

#include "Camera.h"
#include "CoreAssetManager.h"
#include "Texture.h"

class Skybox
{
    friend class Renderer;

private:
    Skybox();
    ~Skybox();

    void render(Shader * shader, Camera * cam);
    void loadImages(std::string * filenames);

    void setSize(float _size) 
    {
        size = _size;
        world = glm::scale(glm::mat4(1.0f), glm::vec3(size));
    }

    glm::mat4 world;

    GLuint vao_id;
    GLuint vbo_ids[2];
    GLuint indices_count;

    Texture * cubeMap;

    float size;
};

