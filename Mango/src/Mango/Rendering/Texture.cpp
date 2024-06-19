#include "mgpch.h"
#include "Texture.h"

#include "glm/common.hpp"
#include "glm/exponential.hpp"

#define STB_IMAGE_IMPLEMENTATION
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
        MG_PROFILE_ZONE_SCOPED;

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
        MG_PROFILE_ZONE_SCOPED;

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
    // --------------------- Texture Sampler -------------------------
    TextureSampler::TextureSampler() : m_so_id(0), m_max_anisotropy(1.0f) {}

    void TextureSampler::create()
    {
        MG_PROFILE_ZONE_SCOPED;

        glCreateSamplers(1, &m_so_id);

        setFiltering(TextureFiltering::MIN,       TextureFilteringParam::LINEAR_MIP_LINEAR);
        setFiltering(TextureFiltering::MAG,       TextureFilteringParam::LINEAR);
        setWraping  (TextureWrapingCoordinate::S, TextureWrapingParam::CLAMP_TO_EDGE);
        setWraping  (TextureWrapingCoordinate::T, TextureWrapingParam::CLAMP_TO_EDGE);
    }

    void TextureSampler::setFiltering(TextureFiltering type, TextureFilteringParam param)
    {
        MG_PROFILE_ZONE_SCOPED;

        if (type == TextureFiltering::MAG && param > TextureFilteringParam::LINEAR)
        {
            param = TextureFilteringParam::LINEAR;
        }

        glSamplerParameteri(m_so_id, GLenum(type), GLint(param));
    }

    void TextureSampler::setMinLod(float min)
    {
        MG_PROFILE_ZONE_SCOPED;

        glSamplerParameterf(m_so_id, GL_TEXTURE_MIN_LOD, min);
    }

    void TextureSampler::setMaxLod(float max)
    {
        MG_PROFILE_ZONE_SCOPED;

        glSamplerParameterf(m_so_id, GL_TEXTURE_MAX_LOD, max);
    }

    void TextureSampler::setWraping(TextureWrapingCoordinate coord, TextureWrapingParam param)
    {
        MG_PROFILE_ZONE_SCOPED;

        glSamplerParameteri(m_so_id, GLenum(coord), GLint(param));
    }

    void TextureSampler::setBorderColor(float r, float g, float b, float a)
    {
        MG_PROFILE_ZONE_SCOPED;

        float color[4] = { r, g, b, a };
        glSamplerParameterfv(m_so_id, GL_TEXTURE_BORDER_COLOR, color);
    }

    void TextureSampler::setCompareMode(TextureCompareMode mode)
    {
        MG_PROFILE_ZONE_SCOPED;

        glSamplerParameteri(m_so_id, GL_TEXTURE_COMPARE_MODE, GLint(mode));
    }

    void TextureSampler::setCompareFunc(TextureCompareFunc func)
    {
        MG_PROFILE_ZONE_SCOPED;

        glSamplerParameteri(m_so_id, GL_TEXTURE_COMPARE_FUNC, GLint(func));
    }

    void TextureSampler::setAnisotropy(float anisotropy)
    {
        MG_PROFILE_ZONE_SCOPED;

        float max_anisotropy;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &max_anisotropy);

        glm::clamp(anisotropy, 1.0f, max_anisotropy);
        glSamplerParameterf(m_so_id, GL_TEXTURE_MAX_ANISOTROPY, anisotropy);
    }

    // --------------------- Texture -------------------------

    void Texture::setFiltering(TextureFiltering type, TextureFilteringParam param)
    {
        MG_PROFILE_ZONE_SCOPED;

        if (type == TextureFiltering::MAG && param > TextureFilteringParam::LINEAR)
        {
            param = TextureFilteringParam::LINEAR;
        }

        glTextureParameteri(m_id, GLenum(type), GLint(param));
    }

    void Texture::setMinLod(float min)
    {
        MG_PROFILE_ZONE_SCOPED;

        glTextureParameterf(m_id, GL_TEXTURE_MIN_LOD, min);
    }

    void Texture::setMaxLod(float max)
    {
        MG_PROFILE_ZONE_SCOPED;

        glTextureParameterf(m_id, GL_TEXTURE_MAX_LOD, max);
    }

    void Texture::setMaxMipmapLevel(int level)
    {
        glTextureParameteri(m_id, GL_TEXTURE_MAX_LEVEL, level);
    }

    void Texture::setSwizzle(const glm::ivec4& swizzle)
    {
        glTextureParameteriv(m_id, GL_TEXTURE_SWIZZLE_RGBA, &swizzle[0]);
    }

    void Texture::setWraping(TextureWrapingCoordinate coord, TextureWrapingParam param)
    {
        MG_PROFILE_ZONE_SCOPED;

        glTextureParameteri(m_id, GLenum(coord), GLint(param));
    }

    void Texture::setBorderColor(float r, float g, float b, float a)
    {
        MG_PROFILE_ZONE_SCOPED;

        float color[4] = { r, g, b, a };
        glTextureParameterfv(m_id, GL_TEXTURE_BORDER_COLOR, color);
    }

    void Texture::setCompareMode(TextureCompareMode mode)
    {
        MG_PROFILE_ZONE_SCOPED;

        glTextureParameteri(m_id, GL_TEXTURE_COMPARE_MODE, GLint(mode));
    }

    void Texture::setCompareFunc(TextureCompareFunc func)
    {
        MG_PROFILE_ZONE_SCOPED;

        glTextureParameteri(m_id, GL_TEXTURE_COMPARE_FUNC, GLint(func));
    }

    void Texture::setAnisotropy(float anisotropy)
    {
        MG_PROFILE_ZONE_SCOPED;

        float max_anisotropy;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &max_anisotropy);

        glm::clamp(anisotropy, 1.0f, max_anisotropy);
        glTextureParameterf(m_id, GL_TEXTURE_MAX_ANISOTROPY, anisotropy);
    }

    // --------------------- Texture creation methods -------------------------

    uint8_t* Texture::load(const std::string& filename, bool isSrgb, bool flip /*= true*/)
    {
        MG_PROFILE_ZONE_SCOPED;

        auto filepath = VFI::getFilepath(filename).generic_string();
        
        if (flip) stbi_set_flip_vertically_on_load(true);

        uint8_t* data;
        if (stbi_is_hdr(filepath.c_str()))
        {
            int width, height, channelsCount;
            data = (uint8_t*)stbi_loadf(filepath.c_str(), &width, &height, &channelsCount, STBI_rgb_alpha);

            if (data)
            {
                m_descriptor.width          = width;
                m_descriptor.height         = height;
                m_descriptor.format         = GL_RGBA;
                m_descriptor.internalFormat = GL_RGBA32F;
                m_descriptor.isSrgb         = isSrgb;
            }
        }
        else
        {
            int width, height, channelsCount;
            data = stbi_load(filepath.c_str(), &width, &height, &channelsCount, STBI_rgb_alpha);

            if (data)
            {
                m_descriptor = createDescriptor(width, height, channelsCount, isSrgb);
            }
        }

        if (flip) stbi_set_flip_vertically_on_load(false);

        return data;
    }

    uint8_t* Texture::load(std::span<uint8_t> buffer, bool isSrgb)
    {
        MG_PROFILE_ZONE_SCOPED;

        uint8_t* data;
        if (stbi_is_hdr_from_memory(buffer.data(), buffer.size()))
        {
            int width, height, channelsCount;
            data = (uint8_t*)stbi_loadf_from_memory(buffer.data(), buffer.size(), &width, &height, &channelsCount, STBI_rgb_alpha);

            if (data)
            {
                m_descriptor.width          = width;
                m_descriptor.height         = height;
                m_descriptor.format         = GL_RGBA;
                m_descriptor.internalFormat = GL_RGBA32F;
                m_descriptor.isSrgb         = isSrgb;
            }
        }
        else
        {
            int width, height, channelsCount;
            data = stbi_load_from_memory(buffer.data(), buffer.size(), &width, &height, &channelsCount, STBI_rgb_alpha);

            if (data)
            {
                m_descriptor = createDescriptor(width, height, channelsCount, isSrgb);
            }
        }

        return data;
    }

    ref<Texture> Texture::create(const TextureDescriptor& descriptor, const std::filesystem::path& filepath)
    {
        return nullptr;
    }

    ref<Texture> Texture::create(const TextureDescriptor& descriptor, const std::filesystem::path filepaths[6])
    {
        return nullptr;
    }

    ref<Texture> Texture::create(const TextureDescriptor& descriptor, std::span<uint8_t> buffer)
    {
        return nullptr;
    }

    bool Texture::createTexture2d(const std::string& filename, bool isSrgb, uint32_t mipmapLevels)
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("Texture::createTexture2d");

        m_filename = filename;
        auto data = load(filename, isSrgb);

        if (!data)
        {
            MG_CORE_ERROR("Texture failed to load at path: {}", VFI::getFilepath(filename).generic_string());
            return false;
        }

        const GLuint maxMipmapLevels = calcMaxMipMapsLevels(m_descriptor.width, m_descriptor.height, 0);
                     mipmapLevels    = mipmapLevels == 0 ? maxMipmapLevels : glm::clamp(mipmapLevels, 1u, maxMipmapLevels);

        glCreateTextures       (GLenum(TextureType::Texture2D), 1, &m_id);
        glTextureStorage2D     (m_id, mipmapLevels, m_descriptor.internalFormat, m_descriptor.width, m_descriptor.height);
        glTextureSubImage2D    (m_id, 0 /* level */, 0 /* xoffset */, 0 /* yoffset */, m_descriptor.width, m_descriptor.height, m_descriptor.format, GL_UNSIGNED_BYTE, data);
        glGenerateTextureMipmap(m_id);

        setFiltering (TextureFiltering::MIN,       TextureFilteringParam::LINEAR_MIP_LINEAR);
        setFiltering (TextureFiltering::MAG,       TextureFilteringParam::LINEAR);
        setWraping   (TextureWrapingCoordinate::S, TextureWrapingParam::REPEAT);
        setWraping   (TextureWrapingCoordinate::T, TextureWrapingParam::REPEAT);
        setAnisotropy(16.0f);

        stbi_image_free(data);

        return true;
    }

    bool Texture::createTexture2dFromMemory(std::span<uint8_t> buffer, bool isSrgb, uint32_t mipmapLevels)
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("Texture::createTexture2dFromMemory");

        auto data = load(buffer, isSrgb);

        if (!data)
        {
            MG_CORE_ERROR("Texture failed to load from the memory.");
            return false;
        }

        const GLuint maxMipmapLevels           = calcMaxMipMapsLevels(m_descriptor.width, m_descriptor.height, 0);
                     m_descriptor.mipMapLevels = mipmapLevels == 0 ? maxMipmapLevels : glm::clamp(mipmapLevels, 1u, maxMipmapLevels);


        glCreateTextures       (GLenum(TextureType::Texture2D), 1, &m_id);
        glTextureStorage2D     (m_id, m_descriptor.mipMapLevels /* levels */, m_descriptor.internalFormat, m_descriptor.width, m_descriptor.height);
        glTextureSubImage2D    (m_id, 0 /* level */, 0 /* xoffset */, 0 /* yoffset */, m_descriptor.width, m_descriptor.height, m_descriptor.format, GL_UNSIGNED_BYTE, data);
        glGenerateTextureMipmap(m_id);

        setFiltering(TextureFiltering::MIN,       TextureFilteringParam::LINEAR_MIP_LINEAR);
        setFiltering(TextureFiltering::MAG,       TextureFilteringParam::LINEAR);
        setWraping  (TextureWrapingCoordinate::S, TextureWrapingParam::REPEAT);
        setWraping  (TextureWrapingCoordinate::T, TextureWrapingParam::REPEAT);

        stbi_image_free(data);

        return true;
    }

    bool Texture::createTexture2dHDR(const std::string & filename, uint32_t mipmapLevels)
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("Texture::createTexture2dHDR");

        m_filename = filename;
        float* data = (float*)load(filename, false);

        if (!data)
        {
            MG_CORE_ERROR("Texture failed to load at path: {}", VFI::getFilepath(filename));
            return false;
        }

        const GLuint maxMipmapLevels = calcMaxMipMapsLevels(m_descriptor.width, m_descriptor.height, 0);
                     mipmapLevels    = mipmapLevels == 0 ? maxMipmapLevels : glm::clamp(mipmapLevels, 1u, maxMipmapLevels);

        glCreateTextures       (GLenum(TextureType::Texture2D), 1, &m_id);
        glTextureStorage2D     (m_id, 1 /* levels */, m_descriptor.internalFormat, m_descriptor.width, m_descriptor.height);
        glTextureSubImage2D    (m_id, 0 /* level */, 0 /* xoffset */, 0 /* yoffset */, m_descriptor.width, m_descriptor.height, m_descriptor.format, GL_FLOAT, data);
        glGenerateTextureMipmap(m_id);

        setFiltering(TextureFiltering::MIN,       TextureFilteringParam::LINEAR);
        setFiltering(TextureFiltering::MAG,       TextureFilteringParam::LINEAR);
        setWraping  (TextureWrapingCoordinate::S, TextureWrapingParam::CLAMP_TO_EDGE);
        setWraping  (TextureWrapingCoordinate::T, TextureWrapingParam::CLAMP_TO_EDGE);
        
        stbi_image_free(data);

        return true;
    }

    bool Texture::createTextureDDS(const std::string& filename)
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("Texture::createTextureDDS");

        m_filename = filename;
        auto filepath = VFI::getFilepath(filename);

        DDSFile dds;
        auto ret = dds.Load(filepath.string().c_str());

        if (Result::Success != ret)
        {
            MG_CORE_ERROR("Texture failed to load at path: {}\nResult: {}", filepath, int(ret));
            return false;
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
        else
        {
            MG_CORE_ERROR("Unsupported DDS texture type. DDS loader only supports textures 1D, 2D and 3D.");
            return false;
        }

        GLFormat format;
        if (!translateDdsFormat(dds.GetFormat(), &format))
        {
            MG_CORE_ERROR("Unsupported format translation. Couldn't translate DDS format to GL format.");
            return false;
        }

        m_descriptor.format         = format.format;
        m_descriptor.internalFormat = format.internalFormat;
        m_descriptor.mipMapLevels   = dds.GetMipCount();
        m_descriptor.width          = dds.GetWidth();
        m_descriptor.height         = dds.GetHeight();
        m_descriptor.depth          = dds.GetDepth();
        m_descriptor.swizzles       = format.swizzle;
        m_descriptor.compressed     = isDdsCompressed(format.format);

        glCreateTextures    (m_descriptor.type, 1, &m_id);

        setFiltering     (TextureFiltering::MIN, TextureFilteringParam::LINEAR_MIP_LINEAR);
        setFiltering     (TextureFiltering::MAG, TextureFilteringParam::LINEAR);
        setWraping       (TextureWrapingCoordinate::S, TextureWrapingParam::REPEAT);
        setWraping       (TextureWrapingCoordinate::T, TextureWrapingParam::REPEAT);
        setMaxMipmapLevel(m_descriptor.mipMapLevels - 1);
        setSwizzle       (format.swizzle);

        glTextureStorage2D(m_id, m_descriptor.mipMapLevels, format.internalFormat, m_descriptor.width, m_descriptor.height);
        dds.Flip();

        for (uint32_t level = 0; level < dds.GetMipCount(); level++)
        {
            auto imageData = dds.GetImageData(level, 0);
            switch (m_descriptor.type)
            {
                case GL_TEXTURE_1D:
                {
                    auto w = imageData->m_width;

                    if (m_descriptor.compressed)
                    {
                        glCompressedTextureSubImage1D(m_id, level, 0, w, format.format, imageData->m_memSlicePitch, imageData->m_mem);
                    }
                    else
                    {
                        glTextureSubImage1D(m_id, level, 0, w, format.format, format.type, imageData->m_mem);
                    }
                    break;
                }

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
                {
                    auto w = imageData->m_width;
                    auto h = imageData->m_height;
                    auto d = imageData->m_depth;

                    if (m_descriptor.compressed)
                    {
                        glCompressedTextureSubImage3D(m_id, level, 0, 0, 0, w, h, d, format.format, imageData->m_memSlicePitch, imageData->m_mem);
                    }
                    else
                    {
                        glTextureSubImage3D(m_id, level, 0, 0, 0, w, h, d, format.format, format.type, imageData->m_mem);
                    }
                    break;
                }
            }
        }

        return true;
    }

    bool Texture::createTextureCubeMap(const std::string* filenames, bool isSrgb, uint32_t mipmapLevels)
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("Texture::createTextureCubeMap");

        constexpr int NUM_FACES = 6;

        uint8_t* images_data[NUM_FACES];

        for (int i = 0; i < NUM_FACES; ++i)
        {
            images_data[i] = load(filenames[i], isSrgb, false);

            if (!images_data[i])
            {
                MG_CORE_ERROR("Texture failed to load at path: {}", VFI::getFilepath(filenames[i]));
                return false;
            }
        }

        const GLuint maxMipmapLevels = calcMaxMipMapsLevels(m_descriptor.width, m_descriptor.height, 0);
                     mipmapLevels    = mipmapLevels == 0 ? maxMipmapLevels : glm::clamp(mipmapLevels, 1u, maxMipmapLevels);

        glCreateTextures  (GLenum(TextureType::TextureCubeMap), 1, &m_id);
        glTextureStorage2D(m_id, mipmapLevels, m_descriptor.internalFormat, m_descriptor.width, m_descriptor.height);

        for (int i = 0; i < NUM_FACES; ++i)
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
                                images_data[i]);
        }

        glGenerateTextureMipmap(m_id);

        setFiltering(TextureFiltering::MIN,       TextureFilteringParam::LINEAR_MIP_LINEAR);
        setFiltering(TextureFiltering::MAG,       TextureFilteringParam::LINEAR);
        setWraping  (TextureWrapingCoordinate::S, TextureWrapingParam::CLAMP_TO_EDGE);
        setWraping  (TextureWrapingCoordinate::T, TextureWrapingParam::CLAMP_TO_EDGE);
        setWraping  (TextureWrapingCoordinate::R, TextureWrapingParam::CLAMP_TO_EDGE);

        for (int i = 0; i < NUM_FACES; ++i)
        {
            stbi_image_free(images_data[i]);
        }

        return true;
    }

    TextureDescriptor Texture::createDescriptor(int width, int height, int channelsCount, bool isSrgb)
    {
        TextureDescriptor desc        = {};
                          desc.width  = width;
                          desc.height = height;

        if (channelsCount == 1)
        {
            desc.format         = GL_RED;
            desc.internalFormat = GL_R8;
        }
        else if (channelsCount == 3)
        {
            desc.format         = GL_RGB;
            desc.internalFormat = isSrgb ? GL_SRGB8 : GL_RGB8;
        }
        else if (channelsCount == 4)
        {
            desc.format         = GL_RGBA;
            desc.internalFormat = isSrgb ? GL_SRGB8_ALPHA8 : GL_RGBA8;
        }

        return desc;
    }

}