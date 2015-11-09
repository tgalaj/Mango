#pragma once

#include <GL\glew.h>
#include <string>
#include <vector>

class Texture
{
public:
    friend class CoreAssetManager;

    void bind(GLenum unit = GL_TEXTURE0);

    void setTypeName(std::string & name);
    const std::string & getTypeName() { return type_name; };

private:
    Texture();
    ~Texture();

    void createTexture2D(std::string filename, GLint base_level);

    std::string type_name;
    GLuint to_id;
    GLenum to_type;
    GLuint width;
    GLuint height;
    GLint  base_level;
    GLenum format;
    GLint internal_format;
};

