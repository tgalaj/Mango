#include <core_engine/CoreAssetManager.h>
#include <framework/window/Window.h>
#include "framework/rendering/BloomPS.h"

namespace mango
{
    void BloomPS::create()
    {
        int width = Window::getWidth();
        int height = Window::getHeight();

        m_brightness_buffer = std::make_shared<RenderTarget>();
        m_brightness_buffer->create(width, height, RenderTarget::ColorInternalFormat::RGB16F, RenderTarget::DepthInternalFormat::NoDepth);

        m_blurred_buffer = std::make_shared<RenderTarget>();
        m_blurred_buffer->create(width, height, RenderTarget::ColorInternalFormat::RGB16F, RenderTarget::DepthInternalFormat::NoDepth);
    }

    void BloomPS::clear()
    {
        m_brightness_buffer->clear();
        m_blurred_buffer->clear();
    }

    void BloomPS::bindBrightnessTexture(GLuint unit)
    {
        m_brightness_buffer->bindTexture(unit, 0);
    }

    void BloomPS::bindBlurredTexture(GLuint unit)
    {
        m_blurred_buffer->bindTexture(unit, 0);
    }

    void BloomPS::extractBrightness(const std::shared_ptr<mango::RenderTarget> &hdr_rt, float threshold)
    {
        m_postprocess->bind();
        m_postprocess->setSubroutine(Shader::Type::FRAGMENT, "extractBrightness");
        m_postprocess->setUniform("threshold", threshold);

        m_brightness_buffer->bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        hdr_rt->bindTexture();

        render();
    }

    void BloomPS::blurGaussian(unsigned int iterations)
    {
        m_postprocess->bind();

        for(unsigned i = 0; i < iterations; ++i)
        {
            m_postprocess->setSubroutine(Shader::Type::FRAGMENT, "blurGaussianHorizontal");
            m_blurred_buffer->bind();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            bindBrightnessTexture(0);
            render();

            m_postprocess->setSubroutine(Shader::Type::FRAGMENT, "blurGaussianVertical");
            m_brightness_buffer->bind();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            bindBlurredTexture(0);
            render();
        }
    }
}