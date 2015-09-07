#include "Texture.h"

Texture::Texture() 
    :  type_name      ("texture_diffuse"),
       to_id          (0),
       to_type        (GL_TEXTURE_2D),
       width          (0),
       height         (0),
       base_level     (0)
{
}


Texture::~Texture()
{
    if (to_id != 0)
    {
        glDeleteTextures(1, &to_id);
        to_id = 0;
    }
}

void Texture::createTexture2D(std::string filename, GLint base_level)
{
    /* Pointer to the image */
    FIBITMAP *dib = CoreAssetManager::loadTexture(filename);
              dib = FreeImage_ConvertTo32Bits(dib);

    /* Pointer to image data */
    BYTE *bits = nullptr;

    bits    = FreeImage_GetBits(dib);
    width   = FreeImage_GetWidth(dib);
    height  = FreeImage_GetHeight(dib);
    int bpp = FreeImage_GetBPP(dib);

    if (bits == 0 || width == 0 || height == 0)
        return;

    to_type         = GL_TEXTURE_2D;
    format          = GL_BGRA;
    internal_format = GL_SRGB8;

    /* Generate GL texture object */
    glGenTextures   (1, &to_id);
    glBindTexture   (to_type, to_id);
    glTexImage2D    (to_type, 
                     base_level, 
                     internal_format, 
                     width, 
                     height, 
                     0 /* border */, 
                     format,
                     GL_UNSIGNED_BYTE, 
                     bits);

    glGenerateMipmap(to_type);
    glTexParameteri (to_type, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri (to_type, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri (to_type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri (to_type, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    /* Release FreeImage data */
    FreeImage_Unload(dib);
}

void Texture::bind(GLenum unit)
{
    glActiveTexture(unit);
    glBindTexture(to_type, to_id);
}

void Texture::setTypeName(std::string & name)
{
    type_name = name;
}
