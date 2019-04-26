#include "framework/rendering/Texture.h"
#include "framework/utilities/Util.h"
#include <iostream>

namespace Vertex
{
    Texture::Texture()
        : m_to_id(0),
          m_to_type(GL_TEXTURE_2D),
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
        unsigned char* data = Util::loadTexture(filename, m_tex_data);

        if(!data)
        {
            std::cout << "Could not load texture " << filename << std::endl;
        }

        m_to_type = GL_TEXTURE_2D;
        m_format  = GL_RGBA;
        m_internal_format = is_srgb ? GL_SRGB8_ALPHA8 : GL_RGBA8;

        /* Generate GL texture object */
        glCreateTextures(m_to_type, 1, &m_to_id);
        glTextureStorage2D(m_to_id,
                           base_level > 0 ? 2 : base_level + 2,
                           m_internal_format,
                           m_tex_data.width,
                           m_tex_data.height);

        glTextureSubImage2D(m_to_id,
                            0 /*level*/,
                            0 /*xoffset*/,
                            0 /*yoffset*/,
                            m_tex_data.width,
                            m_tex_data.height,
                            m_format,
                            GL_UNSIGNED_BYTE,
                            data);

        glGenerateTextureMipmap(m_to_id);

        glTextureParameteri(m_to_id, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(m_to_id, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTextureParameteri(m_to_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(m_to_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTextureParameteri(m_to_id, GL_TEXTURE_MAX_ANISOTROPY, 16); // TODO anisotropy as Renderer parameter

        /* Release image data */
        stbi_image_free(data);
    }

    void Texture::genCubeMapTexture(const std::string * filenames, GLint base_level, bool is_srgb)
    {
        const int numCubeFaces = 6;

        /* Pointer to the image data */
        unsigned char * imgs_data[numCubeFaces];

        for (int i = 0; i < numCubeFaces; ++i)
        {
            imgs_data[i] = Util::loadTexture(filenames[i], m_tex_data);
        }

        m_to_type = GL_TEXTURE_CUBE_MAP;
        m_format = GL_RGBA;
        m_internal_format = is_srgb ? GL_SRGB8_ALPHA8 : GL_RGBA8;

        /* Generate GL texture object */
        glCreateTextures(m_to_type, 1, &m_to_id);
        glTextureStorage2D(m_to_id,
                           base_level > 0 ? 2 : base_level + 2,
                           m_internal_format,
                           m_tex_data.width,
                           m_tex_data.height);

        for (int i = 0; i < numCubeFaces; ++i)
        {
            glTextureSubImage3D(m_to_id,
                                0 /*level*/,
                                0 /*xoffset*/,
                                0 /*yoffset*/,
                                i /*zoffset*/,
                                m_tex_data.width,
                                m_tex_data.height,
                                1 /*depth*/,
                                m_format,
                                GL_UNSIGNED_BYTE,
                                imgs_data[i]);
        }

        glGenerateTextureMipmap(m_to_id);

        glTextureParameteri(m_to_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(m_to_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureParameteri(m_to_id, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTextureParameteri(m_to_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(m_to_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTextureParameteri(m_to_id, GL_TEXTURE_MAX_ANISOTROPY, 16);

        for (int i = 0; i < numCubeFaces; ++i)
        {
            /* Release images' data */
            stbi_image_free(imgs_data[i]);
        }
    }

    void Texture::bind(GLuint unit) const
    {
        glBindTextureUnit(unit, m_to_id);
    }
}
