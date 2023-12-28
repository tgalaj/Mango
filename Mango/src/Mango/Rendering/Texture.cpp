#include "mgpch.h"
#include "Texture.h"

#include "glm/common.hpp"
#include "glm/exponential.hpp"
#include "stb_image.h"

#define TINYDDSLOADER_IMPLEMENTATION
#include "tinyddsloader.h"

using namespace tinyddsloader;

namespace
{
    struct GLFormat
    {
        DDSFile::DXGIFormat dxgiFormat;
        GLenum              type;
        GLenum              format;
        GLenum              internalFormat;
        glm::ivec4          swizzle;
    };

    bool translateDdsFormat(DDSFile::DXGIFormat fmt, GLFormat* outFormat)
    {
        static const glm::ivec4 sws[] =
        {
            { GL_RED,  GL_GREEN, GL_BLUE, GL_ALPHA },
            { GL_BLUE, GL_GREEN, GL_RED,  GL_ALPHA },
            { GL_BLUE, GL_GREEN, GL_RED,  GL_ONE   },
            { GL_RED,  GL_GREEN, GL_BLUE, GL_ONE   },
            { GL_RED,  GL_ZERO,  GL_ZERO, GL_ZERO  },
            { GL_RED,  GL_GREEN, GL_ZERO, GL_ZERO  },
        };

        using DXGIFmt = DDSFile::DXGIFormat;
        static const GLFormat formats[] =
        {
            { DXGIFmt::R8G8B8A8_UNorm,      GL_UNSIGNED_BYTE, GL_RGBA,                                  GL_RGBA8UI,                               sws[0] },
            { DXGIFmt::R8G8B8A8_UNorm_SRGB, GL_UNSIGNED_BYTE, GL_RGBA,                                  GL_SRGB8_ALPHA8,                          sws[0] },
            { DXGIFmt::B8G8R8A8_UNorm,      GL_UNSIGNED_BYTE, GL_RGBA,                                  GL_RGBA8UI,                               sws[1] },
            { DXGIFmt::B8G8R8X8_UNorm,      GL_UNSIGNED_BYTE, GL_RGBA,                                  GL_RGBA8UI,                               sws[2] },
            { DXGIFmt::R32G32_Float,        GL_FLOAT,         GL_RG,                                    GL_RG32F,                                 sws[0] },
            { DXGIFmt::R32G32B32A32_Float,  GL_FLOAT,         GL_RGBA,                                  GL_RGBA32F,                               sws[0] },
            { DXGIFmt::BC1_UNorm,           0,                GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,         GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,         sws[0] },
            { DXGIFmt::BC2_UNorm,           0,                GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,         GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,         sws[0] },
            { DXGIFmt::BC3_UNorm,           0,                GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,         GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,         sws[0] },
            { DXGIFmt::BC4_UNorm,           0,                GL_COMPRESSED_RED_RGTC1_EXT,              GL_COMPRESSED_RED_RGTC1_EXT,              sws[0] },
            { DXGIFmt::BC4_SNorm,           0,                GL_COMPRESSED_SIGNED_RED_RGTC1_EXT,       GL_COMPRESSED_SIGNED_RED_RGTC1_EXT,       sws[0] },
            { DXGIFmt::BC5_UNorm,           0,                GL_COMPRESSED_RED_GREEN_RGTC2_EXT,        GL_COMPRESSED_RED_GREEN_RGTC2_EXT,        sws[0] },
            { DXGIFmt::BC5_SNorm,           0,                GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2_EXT, GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2_EXT, sws[0] },
        };

        for (const auto& format : formats)
        {
            if (format.dxgiFormat == fmt)
            {
                if (outFormat)
                {
                    *outFormat = format;
                }
                return true;
            }
        }

        MG_CORE_ASSERT_MSG(false, "Format not supported.");
        return false;
    }

    bool isDdsCompressed(GLenum fmt)
    {
        switch (fmt)
        {
            case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
            case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
            case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
            case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
            case GL_COMPRESSED_RED_RGTC1_EXT:
            case GL_COMPRESSED_SIGNED_RED_RGTC1_EXT:
            case GL_COMPRESSED_RED_GREEN_RGTC2_EXT:
            case GL_COMPRESSED_SIGNED_RED_GREEN_RGTC2_EXT:
                return true;
            default:
                return false;
        }
    }
}

namespace mango
{
    Texture::Texture()
        : m_id(0)
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

    uint8_t* Texture::loadTexture(const std::string& filename, TextureDescriptorGL& descriptor)
    {
        MG_PROFILE_ZONE_SCOPED;

        auto filepath = VFI::getFilepath(filename);

        int width, height, channelsCount;
        uint8_t* data = stbi_load(filepath.string().c_str(), &width, &height, &channelsCount, 0);
        
        if (data)
        {
            descriptor.width     = width;
            descriptor.height    = height;
            m_descriptor.format  = channelsCount == 4 ? GL_RGBA : GL_RGB;
        }

        return data;
    }

    void Texture::genTexture2D(const std::string& filename, GLuint numMipmaps, bool isSrgb /*= false*/)
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("Texture::genTexture2D");

        /* Pointer to the image */
        unsigned char* data = loadTexture(filename, m_descriptor);

        if (!data)
        {
            MG_CORE_ERROR("Could not load texture {}", filename);
            return;
        }
        
        m_descriptor.type           = GL_TEXTURE_2D;
        m_descriptor.internalFormat = isSrgb ? GL_SRGB8_ALPHA8 : GL_RGBA8;

        const GLuint maxNumMipmaps          = 1 + glm::floor(glm::log2(glm::max(float(m_descriptor.width), float(m_descriptor.height))));
                     m_descriptor.mipLevels = glm::clamp(numMipmaps, 1u, maxNumMipmaps);

        /* Generate GL texture object */
        glCreateTextures(m_descriptor.type, 1, &m_id);
        glTextureStorage2D(m_id,
                           m_descriptor.mipLevels,
                           m_descriptor.internalFormat,
                           m_descriptor.width,
                           m_descriptor.height);

        glTextureSubImage2D(m_id,
                            0 /*level*/,
                            0 /*xoffset*/,
                            0 /*yoffset*/,
                            m_descriptor.width,
                            m_descriptor.height,
                            m_descriptor.format,
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

        m_descriptor.width    = 1;
        m_descriptor.height   = 1;

        m_descriptor.type           = GL_TEXTURE_2D;
        m_descriptor.format         = GL_RGBA;
        m_descriptor.internalFormat = GL_RGBA8;

        GLubyte pixelData[] = { static_cast<GLubyte>(color.r),
                                static_cast<GLubyte>(color.g),
                                static_cast<GLubyte>(color.b),
                                static_cast<GLubyte>(color.a) };

        /* Generate GL texture object */
        glCreateTextures(m_descriptor.type, 1, &m_id);
        glTextureStorage2D(m_id,
                           m_descriptor.mipLevels,
                           m_descriptor.internalFormat,
                           m_descriptor.width,
                           m_descriptor.height);

        glTextureSubImage2D(m_id,
                            0 /*level*/,
                            0 /*xoffset*/,
                            0 /*yoffset*/,
                            m_descriptor.width,
                            m_descriptor.height,
                            m_descriptor.format,
                            GL_UNSIGNED_BYTE,
                            pixelData);

        glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }

    void Texture::genCubeMapTexture(const std::string * filenames, GLuint numMipmaps, bool isSrgb /*= false*/)
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("Texture::genCubeMapTexture");

        const int numCubeFaces = 6;

        /* Pointer to the image data */
        unsigned char * imgsData[numCubeFaces];

        for (int i = 0; i < numCubeFaces; ++i)
        {
            imgsData[i] = loadTexture(filenames[i], m_descriptor);
        }

        m_descriptor.type           = GL_TEXTURE_CUBE_MAP;
        m_descriptor.internalFormat = isSrgb ? GL_SRGB8_ALPHA8 : GL_RGBA8;
       
        const GLuint maxNumMipmaps          = 1 + glm::floor(glm::log2(glm::max(float(m_descriptor.width), float(m_descriptor.height))));
                     m_descriptor.mipLevels = glm::clamp(numMipmaps, 1u, maxNumMipmaps);
        
        /* Generate GL texture object */
        glCreateTextures(m_descriptor.type, 1, &m_id);
        glTextureStorage2D(m_id,
                           m_descriptor.mipLevels,
                           m_descriptor.internalFormat,
                           m_descriptor.width,
                           m_descriptor.height);

        for (int i = 0; i < numCubeFaces; ++i)
        {
            glTextureSubImage3D(m_id,
                                0 /*level*/,
                                0 /*xoffset*/,
                                0 /*yoffset*/,
                                i /*zoffset*/,
                                m_descriptor.width,
                                m_descriptor.height,
                                1 /*depth*/,
                                m_descriptor.format,
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

    TextureDDS::TextureDDS() {}

    void TextureDDS::genTexture(const std::string& filename)
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("TextureCompressed::genTexture");

        auto filepath = VFI::getFilepath(filename);

        DDSFile dds;
        auto ret = dds.Load(filepath.string().c_str());

        if (Result::Success != ret)
        {
            std::cout << "Failed to load.[" << filepath << "]\n";
            std::cout << "Result : " << int(ret) << "\n";

            // TODO assert
            fprintf(stderr, "Texture failed to load at path: %s\n", filepath.string().c_str());
            fprintf(stderr, "Result: %d", int(ret));
            return;
        }

        if (dds.GetTextureDimension() == DDSFile::TextureDimension::Texture2D)
        {
            m_descriptor.type = GL_TEXTURE_2D;
        }
        else if (dds.GetTextureDimension() == DDSFile::TextureDimension::Texture1D)
        {
            m_descriptor.type = GL_TEXTURE_1D;
        }
        else if (dds.GetTextureDimension() == DDSFile::TextureDimension::Texture3D)
        {
            m_descriptor.type = GL_TEXTURE_3D;
        }

        GLFormat format;
        if (!translateDdsFormat(dds.GetFormat(), &format))
        {
            // TODO Assert
            return;
        }

        m_descriptor.format         = format.format;
        m_descriptor.internalFormat = format.internalFormat;
        m_descriptor.mipLevels      = dds.GetMipCount();
        m_descriptor.width          = dds.GetWidth();
        m_descriptor.height         = dds.GetHeight();
        m_descriptor.depth          = dds.GetDepth();
        m_descriptor.swizzles       = format.swizzle;
        m_descriptor.compressed     = isDdsCompressed(format.format);

        glCreateTextures    (m_descriptor.type, 1, &m_id);
        glTextureParameteri (m_id, GL_TEXTURE_BASE_LEVEL, 0);
        glTextureParameteri (m_id, GL_TEXTURE_MAX_LEVEL,  m_descriptor.mipLevels - 1);
        glTextureParameteriv(m_id, GL_TEXTURE_SWIZZLE_R,  &format.swizzle[0]);

        glTextureStorage2D(m_id, m_descriptor.mipLevels, format.internalFormat, m_descriptor.width, m_descriptor.height);
        dds.Flip();

        for (uint32_t level = 0; level < dds.GetMipCount(); level++)
        {
            auto imageData = dds.GetImageData(level, 0);
            switch (m_descriptor.type)
            {
                case GL_TEXTURE_1D:
                    // TODO
                    break;

                case GL_TEXTURE_2D:
                {
                    auto w = imageData->m_width;
                    auto h = imageData->m_height;

                    if (m_descriptor.compressed)
                    {
                        glCompressedTextureSubImage2D(m_id, level, 0, 0, w, h, format.format, imageData->m_memSlicePitch, imageData->m_mem);
                    }
                    else
                    {
                        glTextureSubImage2D(m_id, level, 0, 0, w, h, format.format, format.type, imageData->m_mem);
                    }
                    break;
                }

                case GL_TEXTURE_3D:
                    // TODO
                    break;
            }
        }
    }
}
