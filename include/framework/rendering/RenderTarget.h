#pragma once

#include <glad/glad.h>

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
            NoDepth
        };

        enum ColorType
        {
            RGB888,
            RGBA8888,
            HDR = 0xFFFFFF,
            RG16F,
            RGB16F,
            RGBA16F
        };

        enum RenderTargetType
        {
            Tex2D   = GL_TEXTURE_2D,
            TexCube = GL_TEXTURE_CUBE_MAP
        };

        RenderTarget();
        ~RenderTarget();

        /* Creates Render Target with Color and Depth attachments */
        void create(unsigned width, unsigned height, ColorType color, DepthType depth, RenderTargetType rt_type = Tex2D, bool use_filtering = true);

        /* Creates Render Target with Depth attachment aka Shadow Map */
        void create(unsigned width, unsigned height, DepthType depth, RenderTargetType rt_type = Tex2D, bool use_filtering = true);

        void bind() const;
        void bindTexture(GLuint texture_unit = 0) const;
        void releaseTexture() const { glBindTexture(GL_TEXTURE_2D, 0); }

        bool validate() const;

        unsigned getWidth()  const { return m_width;  }
        unsigned getHeight() const { return m_height; }

    private:
        GLuint m_fbo_id;
        GLuint m_to_id;
        GLuint m_depth_rbo_id;
        GLuint m_width, m_height;
        GLenum m_type;
    };
}