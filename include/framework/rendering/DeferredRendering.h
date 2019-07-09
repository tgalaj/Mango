#pragma once
#include "PostprocessEffect.h"
#include "RenderTarget.h"

namespace Vertex
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
        
        void createGBuffer();
        void clearGBuffer();

        void bindGBuffer();
        void bindGBufferReadOnly();

        void bindGBufferTexture(GLuint unit, GLuint gbuffer_property_id);
        void bindGBufferTextures();

    private:
        std::shared_ptr<RenderTarget> m_gbuffer;
    };
}
