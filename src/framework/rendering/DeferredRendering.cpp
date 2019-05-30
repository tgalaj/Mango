#include "framework/rendering/DeferredRendering.h"
#include "core_engine/CoreAssetManager.h"
#include "framework/window/Window.h"

namespace Vertex
{
    void DeferredRendering::init(const std::string & filter_name, const std::string & fragment_shader_path)
    {
        m_postprocess = CoreAssetManager::createShader(filter_name, "FSQ.vert", fragment_shader_path);
        m_postprocess->link();

        m_gbuffer_shader = CoreAssetManager::createShader("GBuffer", "GBuffer.vert", "GBuffer.frag");
        m_gbuffer_shader->link();
    }

    void DeferredRendering::createGBuffer()
    {
        std::vector<RenderTarget::MRTEntry> mrt_entries(5);
        mrt_entries[POSITION] = RenderTarget::MRTEntry(RenderTarget::Color, RenderTarget::RGB16F); 
        mrt_entries[TEXCOORD] = RenderTarget::MRTEntry(RenderTarget::Color, RenderTarget::RGB16F); 
        mrt_entries[NORMAL]   = RenderTarget::MRTEntry(RenderTarget::Color, RenderTarget::RGB16F); 
                         
        mrt_entries[ALBEDO_SPECULAR] = RenderTarget::MRTEntry(RenderTarget::Color, RenderTarget::RGBA16F);
        mrt_entries[DEPTH]           = RenderTarget::MRTEntry(RenderTarget::Depth, RenderTarget::NoColor, RenderTarget::Depth24);

        m_gbuffer = std::make_shared<RenderTarget>();
        m_gbuffer->createMRT(mrt_entries, Window::getWidth(), Window::getHeight());
    }

    void DeferredRendering::bindGBuffer()
    {
        m_gbuffer->bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_gbuffer_shader->bind();
    }

    void DeferredRendering::bindTexture(GLuint unit, GLuint gbuffer_property_id)
    {
        m_gbuffer->bindTexture(unit, gbuffer_property_id);
    }

    void DeferredRendering::bindTextures()
    {
        m_gbuffer->bindTexture(0, POSITION);
        m_gbuffer->bindTexture(1, TEXCOORD);
        m_gbuffer->bindTexture(2, NORMAL);
        m_gbuffer->bindTexture(3, ALBEDO_SPECULAR);
        m_gbuffer->bindTexture(4, DEPTH);
    }
}
