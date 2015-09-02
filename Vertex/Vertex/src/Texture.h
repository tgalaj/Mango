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

    void createTexture2D(std::string filename, GLint base_level = 0);
    void bind(GLenum unit = GL_TEXTURE0);

    void setTypeName(std::string & name);

private:
    static GLuint next_free_to_unit;

    std::string type_name;
    GLuint to_id;
    GLenum to_type;
    GLuint width;
    GLuint height;
    GLint  base_level;
    GLenum format;
    GLint internal_format;
};

