#pragma once
#include "PostprocessEffect.h"
#include "RenderTarget.h"
#include "DeferredRendering.h"

namespace mango
{
    class SSAO : public PostprocessEffect
    {
    public:
        enum class BufferPropertyName { BRIGHTNESS = 0,
                                        BLURRED    = 1};

        SSAO();
        ~SSAO();

        void init(const std::string &filter_name, const std::string &fragment_shader_path) override;

        void create();
        void clear();

        void bindSSAOTexture(GLuint unit);
        void bindBlurredSSAOTexture(GLuint unit);

        void computeSSAO(const std::shared_ptr<mango::DeferredRendering> & g_buffer, const glm::mat4 & view, const glm::mat4 & projection);
        void blurSSAO();

        void setKernelSize(unsigned kernel_size) { m_kernel_size = kernel_size; genKernel(); }
        void setRadius(float radius)             { m_radius = radius; }
        void setBias(float bias)                 { m_bias = bias; }
        void setPower(float power)               { m_power = power; }

    private:
        void cleanGLdata();
        void genKernel();
        void genRandomRotationVectors(unsigned noise_tex_width, unsigned noise_tex_height);

        std::vector<glm::vec3> m_kernel;

        std::shared_ptr<RenderTarget> m_ssao_buffer;
        std::shared_ptr<RenderTarget> m_blurred_buffer;

        GLuint m_noise_to_id;
        GLint m_kernel_size;
        float m_radius;
        float m_bias;
        float m_power;

    };
}