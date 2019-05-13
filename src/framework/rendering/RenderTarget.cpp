#include "framework/rendering/RenderTarget.h"
#include <cassert>

namespace Vertex
{
    RenderTarget::RenderTarget()
        : m_fbo_id(0),
          m_to_id(0),
          m_depth_rbo_id(0),
          m_width(0),
          m_height(0),
          m_type(0)
    {
    }

    RenderTarget::~RenderTarget()
    {
        clear();
    }

    void RenderTarget::create(unsigned width, unsigned height, ColorType color, DepthType depth, RenderTargetType rt_type, bool use_filtering)
    {
        glGenFramebuffers(1, &m_fbo_id);
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo_id);
        m_width  = width;
        m_height = height;
        m_type   = rt_type;

        glGenTextures(1, &m_to_id);
        glBindTexture(m_type, m_to_id);

        GLuint color_format = 0;
        GLuint depth_format = 0;

        if (color >= HDR)
        {
            switch(color)
            {
                case RGB16F:  color_format = GL_RGB16F; break;
                default:
                case RGBA16F: color_format = GL_RGBA16F; break;
            }
        }
        else
        {
            switch (color)
            {
                case RGBA8888: color_format = GL_RGBA; break;
                default:
                case RGB888:   color_format = GL_RGB; break;
            }
        }

        glTexParameteri(m_type, GL_TEXTURE_MIN_FILTER, use_filtering ? GL_LINEAR : GL_NEAREST);
        glTexParameteri(m_type, GL_TEXTURE_MAG_FILTER, use_filtering ? GL_LINEAR : GL_NEAREST);

        if (m_type == Tex2D)
        {
            glTexParameterf(m_type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameterf(m_type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glTexStorage2D(m_type, 1 /* levels */, color_format, m_width, m_height);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_type, m_to_id, 0 /* level */);
        }

        if (m_type == TexCube)
        {
            glTexParameteri(m_type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(m_type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(m_type, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

            glTexStorage2D(m_type, 1 /* levels */, color_format, m_width, m_height);
            glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_to_id, 0);
        }

        if(depth != NoDepth)
        {
            glGenRenderbuffers(1, &m_depth_rbo_id);
            glBindRenderbuffer(GL_RENDERBUFFER, m_depth_rbo_id);

            switch (depth)
            {
                case Depth24:  depth_format = GL_DEPTH_COMPONENT24; break;
                case Depth32:  depth_format = GL_DEPTH_COMPONENT32; break;
                case Depth32F: depth_format = GL_DEPTH_COMPONENT32F; break;
                default:
                case Depth16:  depth_format = GL_DEPTH_COMPONENT16; break;
            }

            glRenderbufferStorage(GL_RENDERBUFFER, depth_format, width, height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depth_rbo_id);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void RenderTarget::create(unsigned width, unsigned height, DepthType depth, RenderTargetType rt_type, bool use_filtering)
    {
        assert(depth != NoDepth);

        glGenFramebuffers(1, &m_fbo_id);
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo_id);
        m_width  = width;
        m_height = height;
        m_type   = rt_type;

        glGenTextures(1, &m_to_id);
        glBindTexture(m_type, m_to_id);

        GLuint depth_format = 0;
        switch (depth)
        {
            case Depth24:  depth_format = GL_DEPTH_COMPONENT24; break;
            case Depth32:  depth_format = GL_DEPTH_COMPONENT32; break;
            case Depth32F: depth_format = GL_DEPTH_COMPONENT32F; break;
            default:
            case Depth16:  depth_format = GL_DEPTH_COMPONENT16; break;
        }

        glTexParameteri(m_type, GL_TEXTURE_MIN_FILTER, use_filtering ? GL_LINEAR : GL_NEAREST);
        glTexParameteri(m_type, GL_TEXTURE_MAG_FILTER, use_filtering ? GL_LINEAR : GL_NEAREST);

        /* For sampler<type>Shadow */
        glTexParameteri(m_type, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glTexParameteri(m_type, GL_TEXTURE_COMPARE_FUNC, GL_LESS);

        if (m_type == Tex2D)
        {
            glTexParameteri(m_type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(m_type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

            float border_color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
            glTexParameterfv(m_type, GL_TEXTURE_BORDER_COLOR, border_color);

            glTexStorage2D(m_type, 1 /* levels */, depth_format, m_width, m_height);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_type, m_to_id, 0);
        }

        if(m_type == TexCube)
        {
            glTexParameteri(m_type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(m_type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(m_type, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

            glTexStorage2D(m_type, 1 /* levels */, depth_format, m_width, m_height);
            glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_to_id, 0);
        }

        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void RenderTarget::clear()
    {
        if (m_to_id != 0)
        {
            glDeleteTextures(1, &m_to_id);
            m_to_id = 0;
        }

        if (m_depth_rbo_id != 0)
        {
            glDeleteRenderbuffers(1, &m_depth_rbo_id);
            m_depth_rbo_id = 0;
        }

        if(m_fbo_id != 0)
        {
            glDeleteFramebuffers(1, &m_fbo_id);
            m_fbo_id = 0;
        }
    }

    void RenderTarget::bind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo_id);
        glViewport(0, 0, m_width, m_height);
    }

    void RenderTarget::bindTexture(GLuint texture_unit) const
    {
        glActiveTexture(GL_TEXTURE0 + texture_unit);
        glBindTexture  (m_type, m_to_id);
    }

    bool RenderTarget::validate() const
    {
        return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
    }
}
