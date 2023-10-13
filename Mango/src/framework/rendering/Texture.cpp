#include "framework/rendering/Texture.h"
#include "framework/utilities/Util.h"
#include <iostream>
#include <glm/common.hpp>
#include <glm/exponential.hpp>

namespace mango
{
    Texture::Texture()
        : m_to_id(0),
          m_to_type(GL_TEXTURE_2D),
          m_num_mipmaps(1), 
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

    void Texture::genTexture2D(const std::string & filename, GLuint num_mipmaps, bool is_srgb)
    {
        /* Pointer to the image */
        unsigned char* data = Util::loadTexture(filename, m_tex_data);

        if(!data)
        {
            std::cout << "Could not load texture " << filename << std::endl;
        }

        m_to_type = GL_TEXTURE_2D;
        m_format  = m_tex_data.channels == 4 ? GL_RGBA : GL_RGB;
        m_internal_format = is_srgb ? GL_SRGB8_ALPHA8 : GL_RGBA8;

        const GLuint max_num_mipmaps = 1 + glm::floor(glm::log2(glm::max(float(m_tex_data.width), float(m_tex_data.height))));
        m_num_mipmaps = glm::clamp(num_mipmaps, 1u, max_num_mipmaps);

        /* Generate GL texture object */
        glCreateTextures(m_to_type, 1, &m_to_id);
        glTextureStorage2D(m_to_id,
                           m_num_mipmaps,
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

    void Texture::genTexture2D1x1(const glm::uvec4 & color)
    {
        m_tex_data.width    = 1;
        m_tex_data.height   = 1;
        m_tex_data.channels = 4;

        m_to_type         = GL_TEXTURE_2D;
        m_format          = GL_RGBA;
        m_internal_format = GL_RGBA8;

        GLubyte pixel_data[] = { static_cast<GLubyte>(color.r),
                                 static_cast<GLubyte>(color.g),
                                 static_cast<GLubyte>(color.b),
                                 static_cast<GLubyte>(color.a) };

        /* Generate GL texture object */
        glCreateTextures(m_to_type, 1, &m_to_id);
        glTextureStorage2D(m_to_id,
                           m_num_mipmaps,
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
                            pixel_data);

        glTextureParameteri(m_to_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(m_to_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureParameteri(m_to_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(m_to_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }

    void Texture::genCubeMapTexture(const std::string * filenames, GLuint num_mipmaps, bool is_srgb)
    {
        const int numCubeFaces = 6;

        /* Pointer to the image data */
        unsigned char * imgs_data[numCubeFaces];

        for (int i = 0; i < numCubeFaces; ++i)
        {
            imgs_data[i] = Util::loadTexture(filenames[i], m_tex_data);
        }

        m_to_type         = GL_TEXTURE_CUBE_MAP;
        m_format          = m_tex_data.channels == 4 ? GL_RGBA : GL_RGB;
        m_internal_format = is_srgb ? GL_SRGB8_ALPHA8 : GL_RGBA8;
       
        const GLuint max_num_mipmaps = 1 + glm::floor(glm::log2(glm::max(float(m_tex_data.width), float(m_tex_data.height))));
        m_num_mipmaps = glm::clamp(num_mipmaps, 1u, max_num_mipmaps);
        
        /* Generate GL texture object */
        glCreateTextures(m_to_type, 1, &m_to_id);
        glTextureStorage2D(m_to_id,
                           m_num_mipmaps,
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

    void Texture::unbindTextureUnit(GLuint unit)
    {
        glBindTextureUnit(unit, 0);
    }
}
