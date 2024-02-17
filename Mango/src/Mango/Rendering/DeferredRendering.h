#pragma once
#include "PostprocessEffect.h"
#include "RenderTarget.h"

namespace mango
{
    class DeferredRendering : public PostprocessEffect
    {
    public:
        enum class GBufferPropertyName { POSITION        = 0, 
                                         NORMAL          = 1, 
                                         ALBEDO_SPECULAR = 2, 
                                         DEPTH           = 3 };

        DeferredRendering() = default;

        void init();
        
        void createGBuffer(int width, int height);
        void clearGBuffer();

        void bindGBuffer();
        void bindGBufferReadOnly();

        void bindGBufferTexture(GLuint unit, GLuint gbufferPropertyID);
        void bindGBufferTextures();

    private:
        ref<RenderTarget> m_gbuffer;
    };
}
