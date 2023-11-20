#pragma once
#include "PostprocessEffect.h"

namespace mango
{
    class DeferredRendering;
    class RenderTarget;

    class SSAO : public PostprocessEffect
    {
    public:
        enum class BufferPropertyName { BRIGHTNESS = 0,
                                        BLURRED    = 1};

        SSAO();
        ~SSAO();

        void init(const std::string & filterName, const std::filesystem::path & fragmentShaderFilepath) override;

        void create();
        void clear();

        void bindSSAOTexture(GLuint unit);
        void bindBlurredSSAOTexture(GLuint unit);

        void computeSSAO(const std::shared_ptr<mango::DeferredRendering> & gbuffer, const glm::mat4 & view, const glm::mat4 & projection);
        void blurSSAO();

        void setKernelSize(unsigned kernelSize) { m_kernelSize = kernelSize; genKernel(); }
        void setRadius    (float radius)        { m_radius     = radius;                  }
        void setBias      (float bias)          { m_bias       = bias;                    }
        void setPower     (float power)         { m_power      = power;                   }

    private:
        void cleanGLdata();
        void genKernel();
        void genRandomRotationVectors(unsigned noiseTexWidth, unsigned noiseTexHeight);

        std::vector<glm::vec3> m_kernel;

        std::shared_ptr<RenderTarget> m_ssaoBuffer;
        std::shared_ptr<RenderTarget> m_blurredBuffer;

        GLuint m_noiseTextureID;
        GLint m_kernelSize;
        float m_radius;
        float m_bias;
        float m_power;

    };
}