#include "Texture.h"
#include "CoreAssetManager.h"

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
    
    FreeImage_FlipVertical(dib);

    /* Pointer to image data */
    BYTE *bits = nullptr;

    bits    = FreeImage_GetBits(dib);
    width   = FreeImage_GetWidth(dib);
    height  = FreeImage_GetHeight(dib);

    if (bits == 0 || width == 0 || height == 0)
        return;

    GLboolean isSRGB = false;
    glGetBooleanv(GL_FRAMEBUFFER_SRGB, &isSRGB);

    to_type         = GL_TEXTURE_2D;
    format          = GL_BGRA;
    internal_format = isSRGB ? GL_SRGB8 : GL_RGB8;

    /* Generate GL texture object */
    glCreateTextures   (to_type, 1, &to_id);
    glTextureStorage2D (to_id, 
                        base_level > 0 ? 2 : base_level + 2, 
                        internal_format, 
                        width, 
                        height);
    glTextureSubImage2D(to_id,
                        0 /*level*/, 
                        0 /*xoffset*/, 
                        0 /*yoffset*/, 
                        width, 
                        height, 
                        format, 
                        GL_UNSIGNED_BYTE, 
                        bits);

    glGenerateTextureMipmap(to_id);

    glTextureParameteri(to_id, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(to_id, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTextureParameteri(to_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(to_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    /* Release FreeImage data */
    FreeImage_Unload(dib);
}

void Texture::createCubeMapTexture(const std::string * filenames, GLint base_level)
{
    const int numCubeFaces = 6;

    /* Pointer to the image */
    FIBITMAP * dibs[numCubeFaces];
    
    /* Pointer to image data */
    BYTE * bits [numCubeFaces];

    for (int i = 0; i < numCubeFaces; ++i)
    {
        dibs[i] = CoreAssetManager::loadTexture(filenames[i]);
        dibs[i] = FreeImage_ConvertTo32Bits(dibs[i]);

        FreeImage_FlipHorizontal(dibs[i]);

        bits[i] = FreeImage_GetBits  (dibs[i]);
        width   = FreeImage_GetWidth (dibs[i]);
        height  = FreeImage_GetHeight(dibs[i]);
        
        if (bits[i] == 0 || width == 0 || height == 0)
            return;
    }

    GLboolean isSRGB = false;
    glGetBooleanv(GL_FRAMEBUFFER_SRGB, &isSRGB);

    to_type = GL_TEXTURE_CUBE_MAP;
    format  = GL_BGRA;
    internal_format = isSRGB ? GL_SRGB8 : GL_RGB8;

    /* Generate GL texture object */
    glCreateTextures   (to_type, 1, &to_id);
    glTextureStorage2D (to_id,
                        base_level > 0 ? 2 : base_level + 2, 
                        internal_format, 
                        width, 
                        height);

    for(int i = 0; i < numCubeFaces; ++i)
    {
        glTextureSubImage3D(to_id,
                            0 /*level*/, 
                            0 /*xoffset*/, 
                            0 /*yoffset*/, 
                            i /*zoffset*/,
                            width, 
                            height,
                            1 /*depth*/,
                            format, 
                            GL_UNSIGNED_BYTE, 
                            bits[i]);
    }

    glGenerateTextureMipmap(to_id);

    glTextureParameteri(to_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(to_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(to_id, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTextureParameteri(to_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTextureParameteri(to_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    for(int i = 0; i < numCubeFaces; ++i)
    {
        /* Release FreeImage data */
        FreeImage_Unload(dibs[i]);
    }
}

void Texture::bind(GLenum unit)
{
    glBindTextureUnit(unit, to_id);
}

void Texture::setTypeName(std::string & name)
{
    type_name = name;
}
