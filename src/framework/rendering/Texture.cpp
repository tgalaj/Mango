#include <FreeImage.h>

#include "framework/rendering/Texture.h"
#include "framework/utilities/Util.h"

namespace Vertex
{
    Texture::Texture()
        : m_to_id(0),
          m_to_type(GL_TEXTURE_2D),
          m_width(0),
          m_height(0),
          m_base_level(0), 
          m_format(0),
          m_internal_format(0)
    {
    }

    Texture::~Texture()
    {
        if (m_to_id != 0)
        {
            glDeleteTextures(1, &m_to_id);
            m_to_id = 0;
        }
    }

    void Texture::genTexture2D(const std::string & filename, GLint base_level, bool is_srgb)
    {
        /* Pointer to the image */
        FIBITMAP *dib = Util::loadTexture(filename);
                  dib = FreeImage_ConvertTo32Bits(dib);

        FreeImage_FlipVertical(dib);

        /* Pointer to image data */
        BYTE *bits = nullptr;

        bits     = FreeImage_GetBits(dib);
        m_width  = FreeImage_GetWidth(dib);
        m_height = FreeImage_GetHeight(dib);

        if (bits == 0 || m_width == 0 || m_height == 0)
            return;

        m_to_type = GL_TEXTURE_2D;
        m_format  = GL_BGRA;
        m_internal_format = is_srgb ? GL_SRGB8_ALPHA8 : GL_RGB8;

        /* Generate GL texture object */
        glCreateTextures(m_to_type, 1, &m_to_id);
        glTextureStorage2D(m_to_id,
                           base_level > 0 ? 2 : base_level + 2,
                           m_internal_format,
                           m_width,
                           m_height);

        glTextureSubImage2D(m_to_id,
                            0 /*level*/,
                            0 /*xoffset*/,
                            0 /*yoffset*/,
                            m_width,
                            m_height,
                            m_format,
                            GL_UNSIGNED_BYTE,
                            bits);

        glGenerateTextureMipmap(m_to_id);

        glTextureParameteri(m_to_id, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(m_to_id, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTextureParameteri(m_to_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(m_to_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        /* Release FreeImage data */
        FreeImage_Unload(dib);
    }

    void Texture::genCubeMapTexture(const std::string * filenames, GLint base_level, bool is_srgb)
    {
        const int numCubeFaces = 6;

        /* Pointer to the image */
        FIBITMAP * dibs[numCubeFaces];

        /* Pointer to image data */
        BYTE * bits[numCubeFaces];

        for (int i = 0; i < numCubeFaces; ++i)
        {
            dibs[i] = Util::loadTexture(filenames[i]);
            dibs[i] = FreeImage_ConvertTo32Bits(dibs[i]);

            FreeImage_FlipHorizontal(dibs[i]);

            bits[i] = FreeImage_GetBits(dibs[i]);
            m_width = FreeImage_GetWidth(dibs[i]);
            m_height = FreeImage_GetHeight(dibs[i]);

            if (bits[i] == 0 || m_width == 0 || m_height == 0)
                return;
        }

        m_to_type = GL_TEXTURE_CUBE_MAP;
        m_format = GL_BGRA;
        m_internal_format = is_srgb ? GL_SRGB8_ALPHA8 : GL_RGB8;

        /* Generate GL texture object */
        glCreateTextures(m_to_type, 1, &m_to_id);
        glTextureStorage2D(m_to_id,
                           base_level > 0 ? 2 : base_level + 2,
                           m_internal_format,
                           m_width,
                           m_height);

        for (int i = 0; i < numCubeFaces; ++i)
        {
            glTextureSubImage3D(m_to_id,
                                0 /*level*/,
                                0 /*xoffset*/,
                                0 /*yoffset*/,
                                i /*zoffset*/,
                                m_width,
                                m_height,
                                1 /*depth*/,
                                m_format,
                                GL_UNSIGNED_BYTE,
                                bits[i]);
        }

        glGenerateTextureMipmap(m_to_id);

        glTextureParameteri(m_to_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(m_to_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureParameteri(m_to_id, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTextureParameteri(m_to_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(m_to_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        for (int i = 0; i < numCubeFaces; ++i)
        {
            /* Release FreeImage data */
            FreeImage_Unload(dibs[i]);
        }
    }

    void Texture::bind(GLuint unit) const
    {
        glBindTextureUnit(unit, m_to_id);
    }
}
