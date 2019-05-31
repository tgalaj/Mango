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
        std::vector<RenderTarget::MRTEntry> mrt_entries(4);
        mrt_entries[GLuint(GBufferPropertyName::POSITION)]        = RenderTarget::MRTEntry(RenderTarget::AttachmentType::Color, RenderTarget::ColorInternalFormat::RGB32F); 
        mrt_entries[GLuint(GBufferPropertyName::NORMAL)]          = RenderTarget::MRTEntry(RenderTarget::AttachmentType::Color, RenderTarget::ColorInternalFormat::RGB32F); 
        mrt_entries[GLuint(GBufferPropertyName::ALBEDO_SPECULAR)] = RenderTarget::MRTEntry(RenderTarget::AttachmentType::Color, RenderTarget::ColorInternalFormat::RGBA8);
        mrt_entries[GLuint(GBufferPropertyName::DEPTH)]           = RenderTarget::MRTEntry(RenderTarget::AttachmentType::Depth, RenderTarget::ColorInternalFormat::NoColor, RenderTarget::DepthInternalFormat::DEPTH32F);

        m_gbuffer = std::make_shared<RenderTarget>(); 
        m_gbuffer->createMRT(mrt_entries, Window::getWidth(), Window::getHeight());
    }

    void DeferredRendering::bindGBuffer()
    {
        m_gbuffer->bind(); 
    }

    void DeferredRendering::bindTexture(GLuint unit, GLuint gbuffer_property_id)
    {
        m_gbuffer->bindTexture(unit, gbuffer_property_id);
    }

    void DeferredRendering::bindTextures()
    {
        m_gbuffer->bindTexture(0, GLuint(GBufferPropertyName::POSITION));
        m_gbuffer->bindTexture(1, GLuint(GBufferPropertyName::NORMAL));
        m_gbuffer->bindTexture(2, GLuint(GBufferPropertyName::ALBEDO_SPECULAR));
        m_gbuffer->bindTexture(3, GLuint(GBufferPropertyName::DEPTH));
    }
}
