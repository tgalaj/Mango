#include "mgpch.h"

#include "BloomPS.h"
#include "CoreEngine/CoreAssetManager.h"
#include "Window/Window.h"

namespace mango
{
    void BloomPS::create()
    {
        int width  = Window::getWidth();
        int height = Window::getHeight();

        m_brightnessBuffer = std::make_shared<RenderTarget>();
        m_brightnessBuffer->create(width, height, RenderTarget::ColorInternalFormat::RGB16F, RenderTarget::DepthInternalFormat::NoDepth);

        m_blurredBuffer = std::make_shared<RenderTarget>();
        m_blurredBuffer->create(width, height, RenderTarget::ColorInternalFormat::RGB16F, RenderTarget::DepthInternalFormat::NoDepth);
    }

    void BloomPS::clear()
    {
        m_brightnessBuffer->clear();
        m_blurredBuffer->clear();
    }

    void BloomPS::bindBrightnessTexture(GLuint unit)
    {
        m_brightnessBuffer->bindTexture(unit, 0);
    }

    void BloomPS::bindBlurredTexture(GLuint unit)
    {
        m_blurredBuffer->bindTexture(unit, 0);
    }

    void BloomPS::extractBrightness(const std::shared_ptr<RenderTarget> & hdrRenderTarget, float threshold /*= 1.0f*/)
    {
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
}