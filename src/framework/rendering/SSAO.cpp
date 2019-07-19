#include "framework/rendering/SSAO.h"
#include <core_engine/CoreAssetManager.h>
#include <framework/window/Window.h>

#include <random>

namespace Vertex
{
    SSAO::SSAO()
        : m_noise_to_id(0),
          m_kernel_size(64),
          m_radius     (0.5f),
          m_bias       (0.025f),
          m_power      (2.0f)
    {
    }

    SSAO::~SSAO()
    {
        cleanGLdata();
    }

    void SSAO::init(const std::string &filter_name, const std::string &fragment_shader_path)
    {
        PostprocessEffect::init(filter_name, fragment_shader_path);

        genKernel();
        genRandomRotationVectors(4, 4); // Generates 4x4 texture with random rotation vectors
    }

    void SSAO::create()
    {
        int width = Window::getWidth();
        int height = Window::getHeight();

        m_ssao_buffer = std::make_shared<RenderTarget>();
        m_ssao_buffer->create(width, height, RenderTarget::ColorInternalFormat::R8, RenderTarget::DepthInternalFormat::NoDepth, RenderTarget::RenderTargetType::Tex2D, false);

        m_blurred_buffer = std::make_shared<RenderTarget>();
        m_blurred_buffer->create(width, height, RenderTarget::ColorInternalFormat::R8, RenderTarget::DepthInternalFormat::NoDepth, RenderTarget::RenderTargetType::Tex2D, false);
    }

    void SSAO::clear()
    {
        m_ssao_buffer->clear();
        m_blurred_buffer->clear();
    }

    void SSAO::bindSSAOTexture(GLuint unit)
    {
        m_ssao_buffer->bindTexture(unit, 0);
    }

    void SSAO::bindBlurredSSAOTexture(GLuint unit)
    {
        m_blurred_buffer->bindTexture(unit, 0);
    }

    void SSAO::computeSSAO(const std::shared_ptr<Vertex::DeferredRendering> & g_buffer, const glm::mat4 & view, const glm::mat4 & projection)
    {
        m_postprocess->bind();
        m_postprocess->setSubroutine(Shader::Type::FRAGMENT, "calcSSAO");

        m_postprocess->setUniform("samples", m_kernel.size(), m_kernel.data());
        m_postprocess->setUniform("view", view);
        m_postprocess->setUniform("projection", projection);
        m_postprocess->setUniform("kernel_size", m_kernel_size);
        m_postprocess->setUniform("radius", m_radius);
        m_postprocess->setUniform("bias", m_bias);
        m_postprocess->setUniform("power", m_power);

        m_ssao_buffer->bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        g_buffer->bindGBufferTexture(1, GLuint(DeferredRendering::GBufferPropertyName::POSITION));
        g_buffer->bindGBufferTexture(2, GLuint(DeferredRendering::GBufferPropertyName::NORMAL));
        glBindTextureUnit(3, m_noise_to_id);

        render();
    }

    void SSAO::blurSSAO()
    {
        m_postprocess->bind();
        m_postprocess->setSubroutine(Shader::Type::FRAGMENT, "blurSSAO");

        m_blurred_buffer->bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        bindSSAOTexture(0);
        render();
    }

    void SSAO::cleanGLdata()
    {
        if (m_noise_to_id != 0)
        {
            glDeleteTextures(1, &m_noise_to_id);
            m_noise_to_id = 0;
        }
    }

    void SSAO::genKernel()
    {
        std::uniform_real_distribution<float> random_floats(0.0, 1.0); // random floats between 0.0 - 1.0
        std::default_random_engine generator;

        if(!m_kernel.empty())
        {
            m_kernel.clear();
        }

        for(unsigned i = 0; i < m_kernel_size; ++i)
        {
            glm::vec3 sample(random_floats(generator) * 2.0 - 1.0,
                             random_floats(generator) * 2.0 - 1.0,
                             random_floats(generator));

            sample = glm::normalize(sample);
            sample = sample * random_floats(generator);

            float scale = float(i) / float(m_kernel_size);
            scale = glm::lerp(0.1f, 1.0f, scale * scale);

            m_kernel.push_back(sample * scale);
        }
    }

    void SSAO::genRandomRotationVectors(unsigned noise_tex_width, unsigned noise_tex_height)
    {
        cleanGLdata();

        std::uniform_real_distribution<float> random_floats(0.0, 1.0); // random floats between 0.0 - 1.0
        std::default_random_engine generator;

        std::vector<glm::vec3> noise_data;
        unsigned noise_size = noise_tex_width * noise_tex_height;

        for(unsigned i = 0; i < noise_size; ++i)
        {
            glm::vec3 noise(random_floats(generator) * 2.0 - 1.0,
                            random_floats(generator) * 2.0 - 1.0,
                            0.0);

            noise = glm::normalize(noise);
            noise_data.push_back(noise);
        }

        glCreateTextures(GL_TEXTURE_2D, 1, &m_noise_to_id);
        glTextureStorage2D(m_noise_to_id,
                           1,
                           GL_RGB32F,
                           noise_tex_width,
                           noise_tex_height);

        glTextureSubImage2D(m_noise_to_id,
                            0 /*level*/,
                            0 /*xoffset*/,
                            0 /*yoffset*/,
                            noise_tex_width,
                            noise_tex_height,
                            GL_RGB,
                            GL_FLOAT,
                            noise_data.data());

        glTextureParameteri(m_noise_to_id, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(m_noise_to_id, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTextureParameteri(m_noise_to_id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(m_noise_to_id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }
}