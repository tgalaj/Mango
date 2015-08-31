#pragma once

#include <GL\glew.h>
#include <string>
#include <vector>

#include "CoreAssetManager.h"

class Texture
{
public:
    Texture();
    ~Texture();

    GLuint getTextureUnit();
    void createTexture2D(std::string filename, GLint base_level = 0);
    void bind();

private:
    static GLuint next_free_to_unit;

    GLuint to_id;
    GLuint to_unit;
    GLuint to_type;
    GLuint width;
    GLuint height;
    GLint  base_level;
    GLenum format;
    GLenum internal_format;

};

