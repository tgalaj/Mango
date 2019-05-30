#pragma once

#include <glad/glad.h>
#include <vector>

namespace Vertex
{
    class RenderTarget
    {
    public:
        enum DepthType
        {
            Depth16,
            Depth24,
            Depth32,
            Depth32F,
            Shadow16,
            Shadow24,
            Shadow32,
            Shadow32F,
            NoDepth
        };

        enum ColorType
        {
            RGB888,
            RGBA8888,
            HDR = 0xFFFFFF,
            RG16F,
            RGB16F,
            RGBA16F,
            NoColor
        };

        enum RenderTargetType
        {
            Tex2D   = GL_TEXTURE_2D,
            TexCube = GL_TEXTURE_CUBE_MAP
        };

        enum AttachmentType
        {
            Color = GL_COLOR_ATTACHMENT0,
            Depth = GL_DEPTH_ATTACHMENT
        };

        struct MRTEntry
        {
            AttachmentType   m_attachment_type = Color;
            ColorType        m_color_type      = NoColor;
            DepthType        m_depth_type      = NoDepth;

            MRTEntry() = default;
            MRTEntry(AttachmentType attachment, ColorType color_type = NoColor, DepthType depth_type = NoDepth)
                : m_attachment_type(attachment),
                  m_color_type     (color_type),
                  m_depth_type     (depth_type) {}
        };

        RenderTarget();
        ~RenderTarget();

        /* Creates Render Target with Color attachment and Depth renderbuffer */
        void create(unsigned width, unsigned height, ColorType color, DepthType depth, RenderTargetType rt_type = Tex2D, bool use_filtering = true);

        /* Creates Render Target with Depth attachment only aka Shadow Map */
        void create(unsigned width, unsigned height, DepthType depth, RenderTargetType rt_type = Tex2D, bool use_filtering = true);

        /* Create multiple render targets */
        void createMRT(const std::vector<MRTEntry> & mrt_entries, unsigned width, unsigned height, RenderTargetType rt_type = Tex2D, bool use_filtering = false, DepthType default_renderbuffer_format = Depth24);

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
