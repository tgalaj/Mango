#pragma once
#include "PostprocessEffect.h"
#include "RenderTarget.h"

namespace mango
{
    class BloomPS : public PostprocessEffect
    {
    public:
        enum class BufferPropertyName { BRIGHTNESS = 0,
                                        BLURRED    = 1};

        BloomPS() = default;

        void create(int width, int height);
        void clear();

        void bindBrightnessTexture(GLuint unit);
        void bindBlurredTexture   (GLuint unit);

        void extractBrightness(const std::shared_ptr<RenderTarget> & hdrRenderTarget, float threshold = 1.0f);
        void blurGaussian(uint32_t iterations = 5);

    private:
        std::shared_ptr<RenderTarget> m_brightnessBuffer;
        std::shared_ptr<RenderTarget> m_blurredBuffer;
    };
}