#include "mgpch.h"

#include "DeferredRendering.h"
#include "RenderTarget.h"
#include "SSAO.h"
#include "Mango/Core/AssetManager.h"
#include "Mango/Core/Services.h"
#include "Mango/Window/Window.h"

namespace mango
{
    SSAO::SSAO()
        : m_noiseTextureID(0),
          m_kernelSize    (64),
          m_radius        (0.5f),
          m_bias          (0.025f),
          m_power         (2.0f)
    {
    }

    SSAO::~SSAO()
    {
        cleanGLdata();
    }

    void SSAO::init(const std::string & filterName, const std::string & fragmentShaderFilename)
    {
        MG_PROFILE_ZONE_SCOPED;

        PostprocessEffect::init(filterName, fragmentShaderFilename);

        genKernel();
        genRandomRotationVectors(4, 4); // Generates 4x4 texture with random rotation vectors
    }

    void SSAO::create()
    {
        MG_PROFILE_ZONE_SCOPED;

        auto window = Services::application()->getWindow();
        int  width  = window->getWidth();
        int  height = window->getHeight();

        m_ssaoBuffer = std::make_shared<RenderTarget>();
        m_ssaoBuffer->create(width, height, RenderTarget::ColorInternalFormat::R8, RenderTarget::DepthInternalFormat::NoDepth, RenderTarget::RenderTargetType::Tex2D, false);

        m_blurredBuffer = std::make_shared<RenderTarget>();
        m_blurredBuffer->create(width, height, RenderTarget::ColorInternalFormat::R8, RenderTarget::DepthInternalFormat::NoDepth, RenderTarget::RenderTargetType::Tex2D, false);
    }

    void SSAO::clear()
    {
        MG_PROFILE_ZONE_SCOPED;

        m_ssaoBuffer->clear();
        m_blurredBuffer->clear();
    }

    void SSAO::bindSSAOTexture(GLuint unit)
    {
        MG_PROFILE_ZONE_SCOPED;

        m_ssaoBuffer->bindTexture(unit, 0);
    }

    void SSAO::bindBlurredSSAOTexture(GLuint unit)
    {
        MG_PROFILE_ZONE_SCOPED;
        
        m_blurredBuffer->bindTexture(unit, 0);
    }

    void SSAO::computeSSAO(const std::shared_ptr<mango::DeferredRendering> & gbuffer, const glm::mat4 & view, const glm::mat4 & projection)
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("SSAO::computeSSAO");

        m_postprocess->bind();
        m_postprocess->setSubroutine(Shader::Type::FRAGMENT, "calcSSAO");

        m_postprocess->setUniform("samples", m_kernel.size(), m_kernel.data());
        m_postprocess->setUniform("view", view);
        m_postprocess->setUniform("projection", projection);
        m_postprocess->setUniform("kernel_size", m_kernelSize);
        m_postprocess->setUniform("radius", m_radius);
        m_postprocess->setUniform("bias", m_bias);
        m_postprocess->setUniform("power", m_power);

        m_ssaoBuffer->bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        gbuffer->bindGBufferTexture(1, GLuint(DeferredRendering::GBufferPropertyName::POSITION));
        gbuffer->bindGBufferTexture(2, GLuint(DeferredRendering::GBufferPropertyName::NORMAL));
        glBindTextureUnit(3, m_noiseTextureID);

        render();
    }

    void SSAO::blurSSAO()
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("SSAO::blurSSAO");

        m_postprocess->bind();
        m_postprocess->setSubroutine(Shader::Type::FRAGMENT, "blurSSAO");

        m_blurredBuffer->bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        bindSSAOTexture(0);
        render();
    }

    void SSAO::cleanGLdata()
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("SSAO::cleanGLdata");

        if (m_noiseTextureID != 0)
        {
            glDeleteTextures(1, &m_noiseTextureID);
            m_noiseTextureID = 0;
        }
    }

    void SSAO::genKernel()
    {
        MG_PROFILE_ZONE_SCOPED;

        std::uniform_real_distribution<float> random_floats(0.0, 1.0); // random floats between 0.0 - 1.0
        std::default_random_engine generator;

        if(!m_kernel.empty())
        {
            m_kernel.clear();
        }

        for(unsigned i = 0; i < m_kernelSize; ++i)
        {
            glm::vec3 sample(random_floats(generator) * 2.0 - 1.0,
                             random_floats(generator) * 2.0 - 1.0,
                             random_floats(generator));

            sample = glm::normalize(sample);
            sample = sample * random_floats(generator);

            float scale = float(i) / float(m_kernelSize);
            scale = glm::mix(0.1f, 1.0f, scale * scale);

            m_kernel.push_back(sample * scale);
        }
    }

    void SSAO::genRandomRotationVectors(unsigned noiseTexWidth, unsigned noiseTexHeight)
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("SSAO::genRandomRotationVectors");

        cleanGLdata();

        std::uniform_real_distribution<float> randomFloats(0.0, 1.0); // random floats between 0.0 - 1.0
        std::default_random_engine generator;

        std::vector<glm::vec3> noiseData;
        unsigned noiseSize = noiseTexWidth * noiseTexHeight;

        for(unsigned i = 0; i < noiseSize; ++i)
        {
            glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0,
                            randomFloats(generator) * 2.0 - 1.0,
                            0.0);

            noise = glm::normalize(noise);
            noiseData.push_back(noise);
        }

        glCreateTextures(GL_TEXTURE_2D, 1, &m_noiseTextureID);
        glTextureStorage2D(m_noiseTextureID,
                           1,
                           GL_RGB32F,
                           noiseTexWidth,
                           noiseTexHeight);

        glTextureSubImage2D(m_noiseTextureID,
                            0 /*level*/,
                            0 /*xoffset*/,
                            0 /*yoffset*/,
                            noiseTexWidth,
                            noiseTexHeight,
                            GL_RGB,
                            GL_FLOAT,
                            noiseData.data());

        glTextureParameteri(m_noiseTextureID, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(m_noiseTextureID, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTextureParameteri(m_noiseTextureID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(m_noiseTextureID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }
}