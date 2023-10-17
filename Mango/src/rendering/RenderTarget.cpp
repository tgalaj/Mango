#include "mgpch.h"

#include "rendering/RenderTarget.h"
#include "helpers/Assertions.h"

namespace mango
{
    RenderTarget::RenderTarget()
        : m_fbo_id(0),
          m_to_ids(nullptr),
          m_num_textures(0),
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

    void RenderTarget::create(unsigned width, unsigned height, ColorInternalFormat color, DepthInternalFormat depth, RenderTargetType rt_type, bool use_filtering)
    {
        std::vector<MRTEntry> mrt_entries(1);
        mrt_entries[0] = { AttachmentType::Color, color };

        auto default_depth_format = (depth == DepthInternalFormat::NoDepth) ? DepthInternalFormat::DEPTH24 : depth;

        createMRT(mrt_entries, width, height, rt_type, use_filtering, default_depth_format);
    }

    void RenderTarget::create(unsigned width, unsigned height, DepthInternalFormat depth, RenderTargetType rt_type, bool use_filtering)
    {
        MG_ASSERT(depth != DepthInternalFormat::NoDepth);

        glGenFramebuffers(1, &m_fbo_id);
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo_id);
        m_width  = width;
        m_height = height;
        m_type   = GLenum(rt_type);

        m_num_textures = 1;
        m_to_ids = new GLuint[m_num_textures];

        glGenTextures(m_num_textures, m_to_ids);
        glBindTexture(m_type, m_to_ids[0]);

        GLuint depth_format = GLuint(depth);

        glTexParameteri(m_type, GL_TEXTURE_MIN_FILTER, use_filtering ? GL_LINEAR : GL_NEAREST);
        glTexParameteri(m_type, GL_TEXTURE_MAG_FILTER, use_filtering ? GL_LINEAR : GL_NEAREST);

        /* For sampler<type>Shadow */
        glTexParameteri(m_type, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glTexParameteri(m_type, GL_TEXTURE_COMPARE_FUNC, GL_LESS);

        if (m_type == GLenum(RenderTargetType::Tex2D))
        {
            glTexParameteri(m_type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(m_type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

            float border_color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
            glTexParameterfv(m_type, GL_TEXTURE_BORDER_COLOR, border_color);

            glTexStorage2D(m_type, 1 /* levels */, depth_format, m_width, m_height);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_type, m_to_ids[0], 0);
        }

        if(m_type == GLenum(RenderTargetType::TexCube))
        {
            glTexParameteri(m_type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(m_type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(m_type, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

            glTexStorage2D(m_type, 1 /* levels */, depth_format, m_width, m_height);
            glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_to_ids[0], 0);
        }

        glDrawBuffer(GL_NONE);
    }

    void RenderTarget::createMRT(const std::vector<MRTEntry>& mrt_entries, unsigned width, unsigned height, RenderTargetType rt_type, bool use_filtering, DepthInternalFormat default_renderbuffer_format)
    {
        m_width  = width;
        m_height = height;
        m_type   = GLenum(rt_type);

        m_num_textures = mrt_entries.size();
        m_to_ids = new GLuint[m_num_textures];

        glGenTextures(m_num_textures, m_to_ids);

        for(unsigned i = 0; i < mrt_entries.size(); ++i)
        {
            glBindTexture(m_type, m_to_ids[i]);

            if(mrt_entries[i].m_attachment_type == AttachmentType::Color)
            {
                GLuint color_format = GLuint(mrt_entries[i].m_color_internalformat);

                glTexParameteri(m_type, GL_TEXTURE_MIN_FILTER, use_filtering ? GL_LINEAR : GL_NEAREST);
                glTexParameteri(m_type, GL_TEXTURE_MAG_FILTER, use_filtering ? GL_LINEAR : GL_NEAREST);

                if (m_type == GLenum(RenderTargetType::Tex2D))
                {
                    glTexParameterf(m_type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                    glTexParameterf(m_type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

                    glTexStorage2D(m_type, 1 /* levels */, color_format, m_width, m_height);
                }

                if (m_type == GLenum(RenderTargetType::TexCube))
                {
                    glTexParameteri(m_type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                    glTexParameteri(m_type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                    glTexParameteri(m_type, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

                    glTexStorage2D(m_type, 1 /* levels */, color_format, m_width, m_height);
                }
            }
            else
            if(mrt_entries[i].m_attachment_type == AttachmentType::Depth)
            {
                GLuint depth_format = GLuint(mrt_entries[i].m_depth_internalformat);

                glTexParameteri(m_type, GL_TEXTURE_MIN_FILTER, use_filtering ? GL_LINEAR : GL_NEAREST);
                glTexParameteri(m_type, GL_TEXTURE_MAG_FILTER, use_filtering ? GL_LINEAR : GL_NEAREST);

                if (m_type == GLenum(RenderTargetType::Tex2D))
                {
                    glTexParameteri(m_type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
                    glTexParameteri(m_type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

                    glTexStorage2D(m_type, 1 /* levels */, depth_format, m_width, m_height);
                }

                if(m_type == GLenum(RenderTargetType::TexCube))
                {
                    glTexParameteri(m_type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                    glTexParameteri(m_type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                    glTexParameteri(m_type, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

                    glTexStorage2D(m_type, 1 /* levels */, depth_format, m_width, m_height);
                }
            }
        }

        glGenFramebuffers(1, &m_fbo_id);
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo_id);

        auto draw_buffers = new GLenum[m_num_textures];

        bool has_depth = false;
        for(unsigned i = 0; i < mrt_entries.size(); ++i)
        {
            if(mrt_entries[i].m_attachment_type == AttachmentType::Color)
            {
                draw_buffers[i] = GL_COLOR_ATTACHMENT0 + i;
                if (m_type == GLenum(RenderTargetType::Tex2D))
                {
                    glFramebufferTexture2D(GL_FRAMEBUFFER, draw_buffers[i], m_type, m_to_ids[i], 0);
                }

                if(m_type == GLenum(RenderTargetType::TexCube))
                {
                    glFramebufferTexture(GL_FRAMEBUFFER, draw_buffers[i], m_to_ids[i], 0);
                }
            }
            else
            if(mrt_entries[i].m_attachment_type == AttachmentType::Depth)
            {
                has_depth       = true;
                draw_buffers[i] = GL_NONE;

                GLenum attachment_type = GL_DEPTH_ATTACHMENT;
                GLuint depth_format    = GLuint(mrt_entries[i].m_depth_internalformat);

                switch(depth_format)
                {
                    case GLuint(DepthInternalFormat::DEPTH24_STENCIL8):
                    case GLuint(DepthInternalFormat ::DEPTH32F_STENCIL8):
                        attachment_type = GL_DEPTH_STENCIL_ATTACHMENT;
                        break;
                    case GLuint(DepthInternalFormat::STENCIL_INDEX8):
                        attachment_type = GL_STENCIL_ATTACHMENT;
                        break;
                }

                if (m_type == GLenum(RenderTargetType::Tex2D))
                {
                    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment_type, m_type, m_to_ids[i], 0);
                }

                if(m_type == GLenum(RenderTargetType::TexCube))
                {
                    glFramebufferTexture(GL_FRAMEBUFFER, attachment_type, m_to_ids[i], 0);
                }
            }
        }

        if(!has_depth)
        {
            glGenRenderbuffers(1, &m_depth_rbo_id);
            glBindRenderbuffer(GL_RENDERBUFFER, m_depth_rbo_id);

            GLuint depth_format = GLuint(default_renderbuffer_format);

            glRenderbufferStorage(GL_RENDERBUFFER, depth_format, m_width, m_height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depth_rbo_id);
        }

        glDrawBuffers(m_num_textures, draw_buffers);

        delete[] draw_buffers;
    }

    void RenderTarget::clear()
    {
        if (m_to_ids != nullptr)
        {
            glDeleteTextures(m_num_textures, m_to_ids);

            delete[] m_to_ids;
            m_to_ids = nullptr;
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

    void RenderTarget::bindReadOnly() const
    {
        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo_id);
    }

    void RenderTarget::bindWriteOnly() const
    {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo_id);
    }

    void RenderTarget::bindTexture(GLuint texture_unit, GLuint render_target_id) const
    {
        glActiveTexture(GL_TEXTURE0 + texture_unit);
        glBindTexture  (m_type, m_to_ids[render_target_id]);
    }

    bool RenderTarget::validate() const
    {
        return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
    }
}
