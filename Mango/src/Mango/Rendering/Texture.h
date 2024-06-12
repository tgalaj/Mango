#pragma once
#include "Mango/Asset/Asset.h"

#include <cmath>
#include <string>

#include "glad/glad.h"
#include "glm/vec4.hpp"

namespace mango
{
    enum class TextureType              { NONE = 0, Texture2D = GL_TEXTURE_2D, TextureCubeMap = GL_TEXTURE_CUBE_MAP };
    enum class TextureFiltering         { MAG                  = GL_TEXTURE_MAG_FILTER,
                                          MIN                  = GL_TEXTURE_MIN_FILTER };
    enum class TextureFilteringParam    { NEAREST              = GL_NEAREST,
                                          LINEAR               = GL_LINEAR, 
                                          NEAREST_MIP_NEAREST  = GL_NEAREST_MIPMAP_NEAREST, 
                                          LINEAR_MIP_NEAREST   = GL_LINEAR_MIPMAP_NEAREST,
                                          NEAREST_MIP_LINEAR   = GL_NEAREST_MIPMAP_LINEAR,
                                          LINEAR_MIP_LINEAR    = GL_LINEAR_MIPMAP_LINEAR };
    enum class TextureWrapingCoordinate { S                    = GL_TEXTURE_WRAP_S,
                                          T                    = GL_TEXTURE_WRAP_T,
                                          R                    = GL_TEXTURE_WRAP_R };
    enum class TextureWrapingParam      { REPEAT               = GL_REPEAT,
                                          MIRRORED_REPEAT      = GL_MIRRORED_REPEAT,
                                          CLAMP_TO_EDGE        = GL_CLAMP_TO_EDGE,
                                          CLAMP_TO_BORDER      = GL_CLAMP_TO_BORDER,
                                          MIRROR_CLAMP_TO_EDGE = GL_MIRROR_CLAMP_TO_EDGE };
    enum class TextureCompareMode       { NONE                 = GL_NONE,
                                          REF                  = GL_COMPARE_REF_TO_TEXTURE };
    enum class TextureCompareFunc       { NEVER                = GL_NEVER,
                                          ALWAYS               = GL_ALWAYS, 
                                          LEQUAL               = GL_LEQUAL, 
                                          GEQUAL               = GL_GEQUAL, 
                                          LESS                 = GL_LESS, 
                                          GREATER              = GL_GREATER, 
                                          EQUAL                = GL_EQUAL, 
                                          NOTEQUAL             = GL_NOTEQUAL };

    class TextureSampler final
    {
    public:
        TextureSampler();
        ~TextureSampler() { release(); };

        TextureSampler(const TextureSampler&) = delete;
        TextureSampler& operator=(const TextureSampler&) = delete;

        TextureSampler(TextureSampler&& other) noexcept : m_so_id(other.m_so_id), m_max_anisotropy(other.m_max_anisotropy)
        {
            other.m_so_id = 0;
        }

        TextureSampler& operator=(TextureSampler&& other) noexcept
        {
            if (this != &other)
            {
                release();
                std::swap(m_so_id, other.m_so_id);
                std::swap(m_max_anisotropy, other.m_max_anisotropy);
            }

            return *this;
        }
        
        void create();
        void setFiltering(TextureFiltering type, TextureFilteringParam param);
        void setMinLod(float min);
        void setMaxLod(float max);
        void setWraping(TextureWrapingCoordinate coord, TextureWrapingParam param);
        void setBorderColor(float r, float g, float b, float a);
        void setCompareMode(TextureCompareMode mode);
        void setCompareFunc(TextureCompareFunc func);
        void setAnisotropy(float anisotropy);

        void bind(uint32_t texture_unit) { glBindSampler(texture_unit, m_so_id); }

    private:
        void release()
        {
            glDeleteSamplers(1, &m_so_id);
            m_so_id = 0;
        }

        GLuint m_so_id;
        float  m_max_anisotropy;
    };

    struct TextureDescriptor
    {
        TextureDescriptor() {}

        GLenum             type           = 0;
        GLenum             format         = 0;
        GLenum             internalFormat = 0;
        GLuint             mipLevels      = 1;
        GLuint             width          = 0;
        GLuint             height         = 0;
        GLuint             depth          = 1;
        glm::ivec4         swizzles       = glm::ivec4(GL_TEXTURE_SWIZZLE_R, GL_TEXTURE_SWIZZLE_G, GL_TEXTURE_SWIZZLE_B, GL_TEXTURE_SWIZZLE_A);
        bool               compressed     = false;
    };

    class Texture final : public Asset
    {
    public:
        Texture() : m_id(0) {}
        ~Texture() { release(); };

        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;

        Texture(Texture&& other) noexcept : m_descriptor(other.m_descriptor), m_id(other.m_id)
        {
            other.m_id = 0;
        }

        Texture& operator=(Texture&& other) noexcept
        {
            if (this != &other)
            {
                release();

                std::swap(m_descriptor, other.m_descriptor);
                std::swap(m_id, other.m_id);
            }

            return *this;
        }

        void bind             (uint32_t unit) const { glBindTextureUnit(unit, m_id); }
        void setFiltering     (TextureFiltering type, TextureFilteringParam param);
        void setMinLod        (float min);
        void setMaxLod        (float max);
        void setMaxMipmapLevel(int level);
        void setSwizzle       (const glm::ivec4& swizzle);
        void setWraping       (TextureWrapingCoordinate coord, TextureWrapingParam param);
        void setBorderColor   (float r, float g, float b, float a);
        void setCompareMode   (TextureCompareMode mode);
        void setCompareFunc   (TextureCompareFunc func);
        void setAnisotropy    (float anisotropy);

        bool createTexture2d          (const std::string& filename, bool isSrgb = false, uint32_t mipmapLevels = 0);
        bool createTexture2d1x1       (const glm::uvec4& color);
        bool createTexture2dFromMemory(uint8_t* memory_data, uint64_t dataSize, bool isSrgb = false, uint32_t mipmapLevels = 0);
        bool createTexture2dHDR       (const std::string& filename, uint32_t mipmapLevels = 0);
        bool createTextureDDS         (const std::string& filename);
        bool createTextureCubeMap     (const std::string* filenames, bool isSrgb = false, uint32_t mipmapLevels = 0);

        TextureDescriptor getDescriptor() const { return m_descriptor; }
        std::string&      getFilename()         { return m_filename;   }
        uint32_t          getRendererID() const { return m_id;         }

        void setName(const std::string& name) { m_filename = name; }

        static uint8_t calcMaxMipMapsLevels(uint32_t width, uint32_t height, uint32_t depth)
        {
            uint8_t num_levels = 1 + std::floor(std::log2(std::max(width, std::max(height, depth))));
            return  num_levels;
        }

        static  TextureDescriptor createDescriptor(int width, int height, int channelsCount, bool isSrgb);

        static AssetType getStaticAssetType() { return AssetType::Texture; };
               AssetType getAssetType() const override { return getStaticAssetType(); };

    private:
        uint8_t* load(const std::string& filename, bool isSrgb, bool flip = true);
        uint8_t* load(uint8_t* memoryData, uint64_t dataSize, bool isSrgb);
        float*   loadf(const std::string& filename, bool flip = true);

        void release()
        {
            glDeleteTextures(1, &m_id);
            m_id = 0;
        }

    private:
        std::string       m_filename   {};
        TextureDescriptor m_descriptor {};
        GLuint            m_id         {};

    private:
        friend class RenderTarget;
    };
}