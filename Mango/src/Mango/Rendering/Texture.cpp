#include "mgpch.h"

#include "glm/common.hpp"
#include "glm/exponential.hpp"
#include "stb_image.h"

#include "Texture.h"

namespace mango
{
    Texture::Texture()
        : m_id            (0),
          m_type          (GL_TEXTURE_2D),
          m_numMipmaps    (1), 
          m_format        (0),
          m_internalFormat(0)
    {
    }

    Texture::~Texture()
    {
        if (m_id != 0)
        {
            glDeleteTextures(1, &m_id);
            m_id = 0;
        }
    }

    unsigned char* Texture::loadTexture(const std::filesystem::path& filepath, ImageData& imageData)
    {
        MG_PROFILE_ZONE_SCOPED;

        int width, height, channelsCount;
        unsigned char* data = stbi_load(filepath.string().c_str(), &width, &height, &channelsCount, 0);

        imageData.width    = width;
        imageData.height   = height;
        imageData.channels = channelsCount;

        return data;
    }

    void Texture::genTexture2D(const std::filesystem::path& filepath, GLuint numMipmaps, bool isSrgb /*= false*/)
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("Texture::genTexture2D");

        /* Pointer to the image */
        unsigned char* data = loadTexture(filepath, m_texData);

        if(!data)
        {
            std::cout << "Could not load texture " << filepath << std::endl;
        }

        m_type           = GL_TEXTURE_2D;
        m_format         = m_texData.channels == 4 ? GL_RGBA : GL_RGB;
        m_internalFormat = isSrgb ? GL_SRGB8_ALPHA8 : GL_RGBA8;

        const GLuint maxNumMipmaps = 1 + glm::floor(glm::log2(glm::max(float(m_texData.width), float(m_texData.height))));
                     m_numMipmaps  = glm::clamp(numMipmaps, 1u, maxNumMipmaps);

        /* Generate GL texture object */
        glCreateTextures(m_type, 1, &m_id);
        glTextureStorage2D(m_id,
                           m_numMipmaps,
                           m_internalFormat,
                           m_texData.width,
                           m_texData.height);

        glTextureSubImage2D(m_id,
                            0 /*level*/,
                            0 /*xoffset*/,
                            0 /*yoffset*/,
                            m_texData.width,
                            m_texData.height,
                            m_format,
                            GL_UNSIGNED_BYTE,
                            data);

        glGenerateTextureMipmap(m_id);

        glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTextureParameteri(m_id, GL_TEXTURE_MAX_ANISOTROPY, 16); // TODO anisotropy as Renderer parameter

        /* Release image data */
        stbi_image_free(data);
    }

    void Texture::genTexture2D1x1(const glm::uvec4 & color)
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("Texture::genTexture2D1x1");

        m_texData.width    = 1;
        m_texData.height   = 1;
        m_texData.channels = 4;

        m_type           = GL_TEXTURE_2D;
        m_format         = GL_RGBA;
        m_internalFormat = GL_RGBA8;

        GLubyte pixelData[] = { static_cast<GLubyte>(color.r),
                                static_cast<GLubyte>(color.g),
                                static_cast<GLubyte>(color.b),
                                static_cast<GLubyte>(color.a) };

        /* Generate GL texture object */
        glCreateTextures(m_type, 1, &m_id);
        glTextureStorage2D(m_id,
                           m_numMipmaps,
                           m_internalFormat,
                           m_texData.width,
                           m_texData.height);

        glTextureSubImage2D(m_id,
                            0 /*level*/,
                            0 /*xoffset*/,
                            0 /*yoffset*/,
                            m_texData.width,
                            m_texData.height,
                            m_format,
                            GL_UNSIGNED_BYTE,
                            pixelData);

        glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }

    void Texture::genCubeMapTexture(const std::filesystem::path * filepaths, GLuint numMipmaps, bool isSrgb /*= false*/)
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("Texture::genCubeMapTexture");

        const int numCubeFaces = 6;

        /* Pointer to the image data */
        unsigned char * imgsData[numCubeFaces];

        for (int i = 0; i < numCubeFaces; ++i)
        {
            imgsData[i] = loadTexture(filepaths[i], m_texData);
        }

        m_type         = GL_TEXTURE_CUBE_MAP;
        m_format          = m_texData.channels == 4 ? GL_RGBA : GL_RGB;
        m_internalFormat = isSrgb ? GL_SRGB8_ALPHA8 : GL_RGBA8;
       
        const GLuint maxNumMipmaps = 1 + glm::floor(glm::log2(glm::max(float(m_texData.width), float(m_texData.height))));
                     m_numMipmaps  = glm::clamp(numMipmaps, 1u, maxNumMipmaps);
        
        /* Generate GL texture object */
        glCreateTextures(m_type, 1, &m_id);
        glTextureStorage2D(m_id,
                           m_numMipmaps,
                           m_internalFormat,
                           m_texData.width,
                           m_texData.height);

        for (int i = 0; i < numCubeFaces; ++i)
        {
            glTextureSubImage3D(m_id,
                                0 /*level*/,
                                0 /*xoffset*/,
                                0 /*yoffset*/,
                                i /*zoffset*/,
                                m_texData.width,
                                m_texData.height,
                                1 /*depth*/,
                                m_format,
                                GL_UNSIGNED_BYTE,
                                imgsData[i]);
        }

        glGenerateTextureMipmap(m_id);

        glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureParameteri(m_id, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTextureParameteri(m_id, GL_TEXTURE_MAX_ANISOTROPY, 16);

        for (int i = 0; i < numCubeFaces; ++i)
        {
            /* Release images' data */
            stbi_image_free(imgsData[i]);
        }
    }

    void Texture::bind(GLuint unit) const
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("Texture::bind");

        glBindTextureUnit(unit, m_id);
    }

    void Texture::unbindTextureUnit(GLuint unit)
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("Texture::unbindTextureUnit");

        glBindTextureUnit(unit, 0);
    }
}
