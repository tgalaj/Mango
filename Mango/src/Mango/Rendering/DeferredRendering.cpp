#include "mgpch.h"

#include "DeferredRendering.h"
#include "Mango/Core/AssetManager.h"
#include "Mango/Core/Services.h"
#include "Mango/Window/Window.h"

namespace mango
{
    void DeferredRendering::init()
{
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("DeferredRendering::init");

        m_postprocess = AssetManager::createShader("GBuffer", "GBuffer.vert", "GBuffer.frag");
        m_postprocess->link();
    }

    void DeferredRendering::createGBuffer()
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("DeferredRendering::createGBuffer");

        std::vector<RenderTarget::MRTEntry> mrtEntries(4);
        mrtEntries[GLuint(GBufferPropertyName::POSITION)]        = RenderTarget::MRTEntry(RenderTarget::AttachmentType::Color, RenderTarget::ColorInternalFormat::RGB32F); 
        mrtEntries[GLuint(GBufferPropertyName::NORMAL)]          = RenderTarget::MRTEntry(RenderTarget::AttachmentType::Color, RenderTarget::ColorInternalFormat::RGB32F); 
        mrtEntries[GLuint(GBufferPropertyName::ALBEDO_SPECULAR)] = RenderTarget::MRTEntry(RenderTarget::AttachmentType::Color, RenderTarget::ColorInternalFormat::RGBA8);
        mrtEntries[GLuint(GBufferPropertyName::DEPTH)]           = RenderTarget::MRTEntry(RenderTarget::AttachmentType::Depth, RenderTarget::ColorInternalFormat::NoColor, RenderTarget::DepthInternalFormat::DEPTH32F);

        auto window = Services::application()->getWindow();

        m_gbuffer = std::make_shared<RenderTarget>();
        m_gbuffer->createMRT(mrtEntries, window->getWidth(), window->getHeight());
    }

    void DeferredRendering::clearGBuffer()
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("DeferredRendering::clearGBuffer");

        m_gbuffer->clear();
    }

    void DeferredRendering::bindGBuffer()
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("DeferredRendering::bindGBuffer");

        m_gbuffer->bind(); 
    }

    void DeferredRendering::bindGBufferReadOnly()
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("DeferredRendering::bindGBufferReadOnly");

        m_gbuffer->bindReadOnly();
    }

    void DeferredRendering::bindGBufferTexture(GLuint unit, GLuint gbufferPropertyID)
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("DeferredRendering::bindGBufferTexture");

        m_gbuffer->bindTexture(unit, gbufferPropertyID);
    }

    void DeferredRendering::bindGBufferTextures()
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("DeferredRendering::bindGBufferTextures");

        m_gbuffer->bindTexture(0, GLuint(GBufferPropertyName::POSITION));
        m_gbuffer->bindTexture(1, GLuint(GBufferPropertyName::NORMAL));
        m_gbuffer->bindTexture(2, GLuint(GBufferPropertyName::ALBEDO_SPECULAR));
        m_gbuffer->bindTexture(3, GLuint(GBufferPropertyName::DEPTH));
    }
}
