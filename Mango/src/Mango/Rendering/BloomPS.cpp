#include "mgpch.h"

#include "BloomPS.h"
#include "Mango/Core/AssetManager.h"

namespace mango
{
    void BloomPS::create(int width, int height)
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("BloomPS::create");

        m_brightnessBuffer = createRef<RenderTarget>();
        m_brightnessBuffer->create(width, height, RenderTarget::ColorInternalFormat::RGB16F, RenderTarget::DepthInternalFormat::NoDepth);

        m_blurredBuffer = createRef<RenderTarget>();
        m_blurredBuffer->create(width, height, RenderTarget::ColorInternalFormat::RGB16F, RenderTarget::DepthInternalFormat::NoDepth);
    }

    void BloomPS::clear()
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("BloomPS::clear");

        m_brightnessBuffer->clear();
        m_blurredBuffer->clear();
    }

    void BloomPS::bindBrightnessTexture(GLuint unit)
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("BloomPS::bindBrightnessTexture");

        m_brightnessBuffer->bindTexture(unit, 0);
    }

    void BloomPS::bindBlurredTexture(GLuint unit)
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("BloomPS::bindBlurredTexture");

        m_blurredBuffer->bindTexture(unit, 0);
    }

    void BloomPS::extractBrightness(const ref<RenderTarget> & hdrRenderTarget, float threshold /*= 1.0f*/)
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("BloomPS::extractBrightness");

        m_postprocess->bind();
        m_postprocess->setSubroutine(Shader::Type::FRAGMENT, "extractBrightness");
        m_postprocess->setUniform("threshold", threshold);

        m_brightnessBuffer->bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        hdrRenderTarget->bindTexture();

        render();
    }

    void BloomPS::blurGaussian(uint32_t iterations)
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("BloomPS::blurGaussian");

        m_postprocess->bind();

        for(unsigned i = 0; i < iterations; ++i)
        {
            m_postprocess->setSubroutine(Shader::Type::FRAGMENT, "blurGaussianHorizontal");
            m_blurredBuffer->bind();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            bindBrightnessTexture(0);
            render();

            m_postprocess->setSubroutine(Shader::Type::FRAGMENT, "blurGaussianVertical");
            m_brightnessBuffer->bind();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            bindBlurredTexture(0);
            render();
        }
    }

    void BloomPS::clearBuffers()
    {
        m_brightnessBuffer->bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_blurredBuffer->bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

}