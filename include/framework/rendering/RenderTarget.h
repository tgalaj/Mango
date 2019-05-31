#pragma once

#include <glad/glad.h>
#include <vector>

namespace Vertex
{
    class RenderTarget
    {
    public:
        enum class DepthInternalFormat
        {
            DEPTH16            = GL_DEPTH_COMPONENT16,
            DEPTH24            = GL_DEPTH_COMPONENT24,
            DEPTH32            = GL_DEPTH_COMPONENT32,
            DEPTH32F           = GL_DEPTH_COMPONENT32F,
            DEPTH24_STENCIL8   = GL_DEPTH24_STENCIL8,
            DEPTH32F_STENCIL8  = GL_DEPTH32F_STENCIL8,
            STENCIL_INDEX8     = GL_STENCIL_INDEX8,
            NoDepth
        };

        enum class ColorInternalFormat
        {
            R8             = GL_R8,
            R8_SNORM       = GL_R8_SNORM,
            R16            = GL_R16,
            R16_SNORM      = GL_R16_SNORM,
            RG8            = GL_RG8,
            RG8_SNORM      = GL_RG8_SNORM,
            RG16           = GL_RG16,
            RG16_SNORM     = GL_RG16_SNORM,
            R3_G3_B2       = GL_R3_G3_B2,
            RGB4           = GL_RGB4,
            RGB5           = GL_RGB5,	
            RGB8           = GL_RGB8,	
            RGB8_SNORM     = GL_RGB8_SNORM,
            RGB10          = GL_RGB10,	
            RGB12          = GL_RGB12,			
            RGB16_SNORM    = GL_RGB16_SNORM,
            RGBA2          = GL_RGBA2,
            RGBA4          = GL_RGBA4,	
            RGB5_A1        = GL_RGB5_A1,
            RGBA8          = GL_RGBA8,
            RGBA8_SNORM    = GL_RGBA8_SNORM,
            RGB10_A2       = GL_RGB10_A2,
            RGB10_A2UI     = GL_RGB10_A2UI,
            RGBA12         = GL_RGBA12,
            RGBA16         = GL_RGBA16,
            SRGB8          = GL_SRGB8,
            SRGB8_ALPHA8   = GL_SRGB8_ALPHA8,
            R16F           = GL_R16F,
            RG16F          = GL_RG16F,
            RGB16F         = GL_RGB16F,
            RGBA16F        = GL_RGBA16F,
            R32F           = GL_R32F,
            RG32F          = GL_RG32F,
            RGB32F         = GL_RGB32F,
            RGBA32F        = GL_RGBA32F,
            R11F_G11F_B10F = GL_R11F_G11F_B10F,
            RGB9_E5        = GL_RGB9_E5,
            R8I            = GL_R8I,
            R8UI           = GL_R8UI,
            R16I           = GL_R16I,
            R16UI          = GL_R16UI,
            R32I           = GL_R32I,
            R32UI          = GL_R32UI,
            RG8I           = GL_RG8I,
            RG8UI          = GL_RG8UI,
            RG16I          = GL_RG16I,
            RG16UI         = GL_RG16UI,
            RG32I          = GL_RG32I,
            RG32UI         = GL_RG32UI,
            RGB8I          = GL_RGB8I,
            RGB8UI         = GL_RGB8UI,
            RGB16I         = GL_RGB16I,
            RGB16UI        = GL_RGB16UI,
            RGB32I         = GL_RGB32I,
            RGB32UI        = GL_RGB32UI,
            RGBA8I         = GL_RGBA8I,
            RGBA8UI        = GL_RGBA8UI,
            RGBA16I        = GL_RGBA16I,
            RGBA16UI       = GL_RGBA16UI,
            RGBA32I        = GL_RGBA32I,
            RGBA32UI       = GL_RGBA32UI,
            NoColor
        };

        enum class RenderTargetType
        {
            Tex2D   = GL_TEXTURE_2D,
            TexCube = GL_TEXTURE_CUBE_MAP
        };

        enum class AttachmentType
        {
            Color = GL_COLOR_ATTACHMENT0,
            Depth = GL_DEPTH_ATTACHMENT
        };

        struct MRTEntry
        {
            AttachmentType      m_attachment_type      = AttachmentType::Color;
            ColorInternalFormat m_color_internalformat = ColorInternalFormat::NoColor;
            DepthInternalFormat m_depth_internalformat = DepthInternalFormat::NoDepth;

            MRTEntry() = default;
            MRTEntry(AttachmentType attachment, ColorInternalFormat color_type = ColorInternalFormat::NoColor, DepthInternalFormat depth_type = DepthInternalFormat::NoDepth)
                : m_attachment_type(attachment),
                  m_color_internalformat     (color_type),
                  m_depth_internalformat     (depth_type) {}
        };

        RenderTarget();
        ~RenderTarget();

        /* Creates Render Target with Color attachment and Depth renderbuffer */
        void create(unsigned width, unsigned height, ColorInternalFormat color, DepthInternalFormat depth, RenderTargetType rt_type = RenderTargetType::Tex2D, bool use_filtering = true);

        /* Creates Render Target with Depth attachment only aka Shadow Map */
        void create(unsigned width, unsigned height, DepthInternalFormat depth, RenderTargetType rt_type = RenderTargetType::Tex2D, bool use_filtering = true);

        /* Create multiple render targets */
        void createMRT(const std::vector<MRTEntry> & mrt_entries, unsigned width, unsigned height, RenderTargetType rt_type = RenderTargetType::Tex2D, bool use_filtering = false, DepthInternalFormat default_renderbuffer_format = DepthInternalFormat::DEPTH24);

        void clear();

        void bind() const;
        void bindTexture(GLuint texture_unit = 0, GLuint render_target_id = 0) const;
        void releaseTexture() const { glBindTexture(m_type, 0); }

        bool validate() const;

        unsigned getWidth()  const { return m_width;  }
        unsigned getHeight() const { return m_height; }

    private:
        void initTextures(std::vector<MRTEntry> & mrt_entries, bool use_filtering = true);
        void initRenderTargets(const std::vector<MRTEntry> & mrt_entries);

        GLuint m_fbo_id;
        
        GLuint * m_to_ids;
        GLuint m_num_textures;

        GLuint m_depth_rbo_id;

        GLuint m_width, m_height;
        GLenum m_type;
    };
}
