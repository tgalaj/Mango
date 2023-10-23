#include "mgpch.h"

#include "DeferredRendering.h"
#include "Core/CoreAssetManager.h"
#include "Window/Window.h"

namespace mango
{
    void DeferredRendering::init()
{
        m_postprocess = CoreAssetManager::createShader("GBuffer", "GBuffer.vert", "GBuffer.frag");
        m_postprocess->link();
    }

    void DeferredRendering::createGBuffer()
    {
        std::vector<RenderTarget::MRTEntry> mrtEntries(4);
        mrtEntries[GLuint(GBufferPropertyName::POSITION)]        = RenderTarget::MRTEntry(RenderTarget::AttachmentType::Color, RenderTarget::ColorInternalFormat::RGB32F); 
        mrtEntries[GLuint(GBufferPropertyName::NORMAL)]          = RenderTarget::MRTEntry(RenderTarget::AttachmentType::Color, RenderTarget::ColorInternalFormat::RGB32F); 
        mrtEntries[GLuint(GBufferPropertyName::ALBEDO_SPECULAR)] = RenderTarget::MRTEntry(RenderTarget::AttachmentType::Color, RenderTarget::ColorInternalFormat::RGBA8);
        mrtEntries[GLuint(GBufferPropertyName::DEPTH)]           = RenderTarget::MRTEntry(RenderTarget::AttachmentType::Depth, RenderTarget::ColorInternalFormat::NoColor, RenderTarget::DepthInternalFormat::DEPTH32F);

        m_gbuffer = std::make_shared<RenderTarget>();
        m_gbuffer->createMRT(mrtEntries, Window::getWidth(), Window::getHeight());
    }

    void DeferredRendering::clearGBuffer()
    {
        m_gbuffer->clear();
    }

    void DeferredRendering::bindGBuffer()
    {
        m_gbuffer->bind(); 
    }

    void DeferredRendering::bindGBufferReadOnly()
    {
        m_gbuffer->bindReadOnly();
    }

    void DeferredRendering::bindGBufferTexture(GLuint unit, GLuint gbufferPropertyID)
    {
        m_gbuffer->bindTexture(unit, gbufferPropertyID);
    }

    void DeferredRendering::bindGBufferTextures()
    {
        m_gbuffer->bindTexture(0, GLuint(GBufferPropertyName::POSITION));
        m_gbuffer->bindTexture(1, GLuint(GBufferPropertyName::NORMAL));
        m_gbuffer->bindTexture(2, GLuint(GBufferPropertyName::ALBEDO_SPECULAR));
        m_gbuffer->bindTexture(3, GLuint(GBufferPropertyName::DEPTH));
    }
}
