#include "mgpch.h"

#include "RenderTarget.h"

namespace mango
{
    RenderTarget::RenderTarget()
        : m_fbo        (0),
          m_textureID  (nullptr),
          m_numTextures(0),
          m_depthRBO   (0),
          m_width      (0),
          m_height     (0),
          m_type       (0)
    {
    }

    RenderTarget::~RenderTarget()
    {
        clear();
    }

    void RenderTarget::create(unsigned width, unsigned height, ColorInternalFormat color, DepthInternalFormat depth, RenderTargetType rtType, bool useFiltering)
    {
        MG_PROFILE_ZONE_SCOPED;

        std::vector<MRTEntry> mrtEntries(1);
        mrtEntries[0] = { AttachmentType::Color, color };

        auto defaultDepthFormat = (depth == DepthInternalFormat::NoDepth) ? DepthInternalFormat::DEPTH24 : depth;

        createMRT(mrtEntries, width, height, rtType, useFiltering, defaultDepthFormat);
    }

    void RenderTarget::create(unsigned width, unsigned height, DepthInternalFormat depth, RenderTargetType rtType, bool useFiltering)
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("RenderTarget::create");

        MG_CORE_ASSERT(depth != DepthInternalFormat::NoDepth);

        glGenFramebuffers(1, &m_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
        m_width  = width;
        m_height = height;
        m_type   = GLenum(rtType);

        m_numTextures = 1;
        m_textureID = new GLuint[m_numTextures];

        glGenTextures(m_numTextures, m_textureID);
        glBindTexture(m_type, m_textureID[0]);

        GLuint depthFormat = GLuint(depth);

        glTexParameteri(m_type, GL_TEXTURE_MIN_FILTER, useFiltering ? GL_LINEAR : GL_NEAREST);
        glTexParameteri(m_type, GL_TEXTURE_MAG_FILTER, useFiltering ? GL_LINEAR : GL_NEAREST);

        /* For sampler<type>Shadow */
        glTexParameteri(m_type, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
        glTexParameteri(m_type, GL_TEXTURE_COMPARE_FUNC, GL_LESS);

        if (m_type == GLenum(RenderTargetType::Tex2D))
        {
            glTexParameteri(m_type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(m_type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

            float border_color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
            glTexParameterfv(m_type, GL_TEXTURE_BORDER_COLOR, border_color);

            glTexStorage2D(m_type, 1 /* levels */, depthFormat, m_width, m_height);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_type, m_textureID[0], 0);
        }

        if (m_type == GLenum(RenderTargetType::TexCube))
        {
            glTexParameteri(m_type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(m_type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(m_type, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

            glTexStorage2D(m_type, 1 /* levels */, depthFormat, m_width, m_height);
            glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_textureID[0], 0);
        }

        glDrawBuffer(GL_NONE);
    }

    void RenderTarget::createMRT(const std::vector<MRTEntry>& mrtEntries, unsigned width, unsigned height, RenderTargetType rtType, bool useFiltering, DepthInternalFormat defaultRenderbufferFormat)
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("RenderTarget::createMRT");

        m_width  = width;
        m_height = height;
        m_type   = GLenum(rtType);

        m_numTextures = mrtEntries.size();
        m_textureID   = new GLuint[m_numTextures];

        glGenTextures(m_numTextures, m_textureID);

        for (unsigned i = 0; i < mrtEntries.size(); ++i)
        {
            glBindTexture(m_type, m_textureID[i]);

            if (mrtEntries[i].attachmentType == AttachmentType::Color)
            {
                GLuint colorFormat = GLuint(mrtEntries[i].colorInternalFormat);

                glTexParameteri(m_type, GL_TEXTURE_MIN_FILTER, useFiltering ? GL_LINEAR : GL_NEAREST);
                glTexParameteri(m_type, GL_TEXTURE_MAG_FILTER, useFiltering ? GL_LINEAR : GL_NEAREST);

                if (m_type == GLenum(RenderTargetType::Tex2D))
                {
                    glTexParameterf(m_type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                    glTexParameterf(m_type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

                    glTexStorage2D(m_type, 1 /* levels */, colorFormat, m_width, m_height);
                }

                if (m_type == GLenum(RenderTargetType::TexCube))
                {
                    glTexParameteri(m_type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                    glTexParameteri(m_type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                    glTexParameteri(m_type, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

                    glTexStorage2D(m_type, 1 /* levels */, colorFormat, m_width, m_height);
                }
            }
            else
            if (mrtEntries[i].attachmentType == AttachmentType::Depth)
            {
                GLuint depthFormat = GLuint(mrtEntries[i].depthInternalFormat);

                glTexParameteri(m_type, GL_TEXTURE_MIN_FILTER, useFiltering ? GL_LINEAR : GL_NEAREST);
                glTexParameteri(m_type, GL_TEXTURE_MAG_FILTER, useFiltering ? GL_LINEAR : GL_NEAREST);

                if (m_type == GLenum(RenderTargetType::Tex2D))
                {
                    glTexParameteri(m_type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
                    glTexParameteri(m_type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

                    glTexStorage2D(m_type, 1 /* levels */, depthFormat, m_width, m_height);
                }

                if (m_type == GLenum(RenderTargetType::TexCube))
                {
                    glTexParameteri(m_type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                    glTexParameteri(m_type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                    glTexParameteri(m_type, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

                    glTexStorage2D(m_type, 1 /* levels */, depthFormat, m_width, m_height);
                }
            }
        }

        glGenFramebuffers(1, &m_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

        auto drawBuffers = new GLenum[m_numTextures];

        bool hasDepth = false;
        for (unsigned i = 0; i < mrtEntries.size(); ++i)
        {
            if (mrtEntries[i].attachmentType == AttachmentType::Color)
            {
                drawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;
                if (m_type == GLenum(RenderTargetType::Tex2D))
                {
                    glFramebufferTexture2D(GL_FRAMEBUFFER, drawBuffers[i], m_type, m_textureID[i], 0);
                }

                if (m_type == GLenum(RenderTargetType::TexCube))
                {
                    glFramebufferTexture(GL_FRAMEBUFFER, drawBuffers[i], m_textureID[i], 0);
                }
            }
            else
            if (mrtEntries[i].attachmentType == AttachmentType::Depth)
            {
                hasDepth       = true;
                drawBuffers[i] = GL_NONE;

                GLenum attachmentType = GL_DEPTH_ATTACHMENT;
                GLuint depthFormat    = GLuint(mrtEntries[i].depthInternalFormat);

                switch(depthFormat)
                {
                    case GLuint(DepthInternalFormat::DEPTH24_STENCIL8):
                    case GLuint(DepthInternalFormat ::DEPTH32F_STENCIL8):
                        attachmentType = GL_DEPTH_STENCIL_ATTACHMENT;
                        break;
                    case GLuint(DepthInternalFormat::STENCIL_INDEX8):
                        attachmentType = GL_STENCIL_ATTACHMENT;
                        break;
                }

                if (m_type == GLenum(RenderTargetType::Tex2D))
                {
                    glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentType, m_type, m_textureID[i], 0);
                }

                if(m_type == GLenum(RenderTargetType::TexCube))
                {
                    glFramebufferTexture(GL_FRAMEBUFFER, attachmentType, m_textureID[i], 0);
                }
            }
        }

        if (!hasDepth)
        {
            glGenRenderbuffers(1, &m_depthRBO);
            glBindRenderbuffer(GL_RENDERBUFFER, m_depthRBO);

            GLuint depthFormat = GLuint(defaultRenderbufferFormat);

            glRenderbufferStorage(GL_RENDERBUFFER, depthFormat, m_width, m_height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthRBO);
        }

        glDrawBuffers(m_numTextures, drawBuffers);

        delete[] drawBuffers;
    }

    void RenderTarget::clear()
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("RenderTarget::clear");

        if (m_textureID != nullptr)
        {
            glDeleteTextures(m_numTextures, m_textureID);

            delete[] m_textureID;
            m_textureID = nullptr;
        }

        if (m_depthRBO != 0)
        {
            glDeleteRenderbuffers(1, &m_depthRBO);
            m_depthRBO = 0;
        }

        if(m_fbo != 0)
        {
            glDeleteFramebuffers(1, &m_fbo);
            m_fbo = 0;
        }
    }

    void RenderTarget::bind() const
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("RenderTarget::bind");

        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
        glViewport(0, 0, m_width, m_height);
    }

    void RenderTarget::bindReadOnly() const
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("RenderTarget::bindReadOnly");

        glBindFramebuffer(GL_READ_FRAMEBUFFER, m_fbo);
    }

    void RenderTarget::bindWriteOnly() const
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("RenderTarget::bindWriteOnly");

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_fbo);
    }

    void RenderTarget::bindTexture(GLuint textureUnit /*= 0*/, GLuint renderTargetID /*= 0*/) const
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("RenderTarget::bindTexture");

        glActiveTexture(GL_TEXTURE0 + textureUnit);
        glBindTexture  (m_type, m_textureID[renderTargetID]);
    }

    bool RenderTarget::validate() const
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("RenderTarget::validate");

        return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
    }
}
