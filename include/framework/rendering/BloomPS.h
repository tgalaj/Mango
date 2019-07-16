#pragma once
#include "PostprocessEffect.h"
#include "RenderTarget.h"

namespace Vertex
{
    class BloomPS : public PostprocessEffect
    {
    public:
        enum class BufferPropertyName { BRIGHTNESS = 0,
                                        BLURRED    = 1};

        BloomPS() = default;

        void create();
        void clear();

        void bindBrightnessTexture(GLuint unit);
        void bindBlurredTexture(GLuint unit);

        void extractBrightness(const std::shared_ptr<RenderTarget> & hdr_rt, float threshold = 1.0f);
        void blurGaussian(unsigned iterations = 5);

    private:
        std::shared_ptr<RenderTarget> m_brightness_buffer;
        std::shared_ptr<RenderTarget> m_blurred_buffer;
    };
}