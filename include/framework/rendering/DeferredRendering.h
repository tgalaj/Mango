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
        void bindGBuffer();
        void bindLightBuffer();

        void bindGBufferTexture(GLuint unit, GLuint gbuffer_property_id);
        void bindGBufferTextures();
        void bindLightTexture(GLuint unit = 0);

    private:
        std::shared_ptr<RenderTarget> m_gbuffer;
        std::shared_ptr<RenderTarget> m_light_buffer;

        std::shared_ptr<Shader>       m_gbuffer_shader;
    };
}
