#pragma once
#include "PostprocessEffect.h"
#include "RenderTarget.h"

namespace Vertex
{
    class DeferredRendering : public PostprocessEffect
    {
    public:
        enum GBufferPropertyName { POSITION        = 0, 
                                   TEXCOORD        = 1, 
                                   NORMAL          = 2, 
                                   ALBEDO_SPECULAR = 3, 
                                   DEPTH           = 4 };

        DeferredRendering() = default;

        void init(const std::string & filter_name, const std::string & fragment_shader_path) override;
        
        void createGBuffer(); 
        void bindGBuffer();

        void bindTexture(GLuint unit, GLuint gbuffer_property_id);
        void bindTextures();

    private:
        friend class RenderingSystem;

        std::shared_ptr<RenderTarget> m_gbuffer;
        std::shared_ptr<Shader>       m_gbuffer_shader;
    };
}
