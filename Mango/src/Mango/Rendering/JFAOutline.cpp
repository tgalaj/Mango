#include "mgpch.h"
#include "JFAOutline.h"
#include "Mango/Core/AssetManager.h"
#include "Mango/Scene/Entity.h"
#include "Mango/Systems/RenderingSystem.h"

namespace mango
{

    JFAOutline::~JFAOutline()
    {
        clear();
    }

    void JFAOutline::init(int width, int height)
    {
        MG_PROFILE_ZONE_SCOPED;

        resize(width, height);

        m_stencilFillShader = AssetManager::createShader("JfaStencilFill", "jfaOutline/JumpFloodMaskFill.vert", "jfaOutline/JumpFloodStencilFill.frag");
        m_stencilFillShader->link();

        m_maskFillShader = AssetManager::createShader("JfaMaskFill", "jfaOutline/JumpFloodMaskFill.vert", "jfaOutline/JumpFloodMaskFill.frag");
        m_maskFillShader->link();

        m_jumpFloodInitPS = createRef<PostprocessEffect>();
        m_jumpFloodInitPS->init("JfaInit", "jfaOutline/JumpFloodInit.frag");

        m_jumpFloodPS = createRef<PostprocessEffect>();
        m_jumpFloodPS->init("JumpFlood", "jfaOutline/JumpFlood.frag");

        m_jumpFloodOutlinePS = createRef<PostprocessEffect>();
        m_jumpFloodOutlinePS->init("JfaOutline", "jfaOutline/JumpFloodOutline.frag");
    }

    void JFAOutline::clear()
    {
        MG_PROFILE_ZONE_SCOPED;
        m_objectsMaskRT->clear();
        m_jumpFloodBufferA->clear();
        m_jumpFloodBufferB->clear();
    }

    void JFAOutline::resize(int width, int height)
    {
        MG_PROFILE_ZONE_SCOPED;
        m_objectsMaskRT = createRef<RenderTarget>();
        m_objectsMaskRT->create(width, height, RenderTarget::ColorInternalFormat::R16F, RenderTarget::DepthInternalFormat::DEPTH24, RenderTarget::RenderTargetType::Tex2D, false);

        m_jumpFloodBufferA = createRef<RenderTarget>();
        m_jumpFloodBufferA->create(width, height, RenderTarget::ColorInternalFormat::RG16F, RenderTarget::DepthInternalFormat::NoDepth, RenderTarget::RenderTargetType::Tex2D, false);

        m_jumpFloodBufferB = createRef<RenderTarget>();
        m_jumpFloodBufferB->create(width, height, RenderTarget::ColorInternalFormat::RG16F, RenderTarget::DepthInternalFormat::NoDepth, RenderTarget::RenderTargetType::Tex2D, false);

        Services::renderer()->addDebugTexture("JfaObjectsMask", m_objectsMaskRT->getTexture(0));
        Services::renderer()->addDebugTexture("JfaBufferA",     m_jumpFloodBufferA->getTexture(0));
        Services::renderer()->addDebugTexture("JfaBufferB",     m_jumpFloodBufferB->getTexture(0));
    }

    void JFAOutline::render(ref<RenderTarget>& dstRT, Entity entity, const glm::vec3& outlineColor, float outlineWidth)
    {
        MG_PROFILE_ZONE_SCOPED;

        // 0. Draw the entity to stencil buffer, so later, 
        // we can cut out the silhouette of the entities in the outline
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_STENCIL_TEST);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

        dstRT->bind();
        glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        renderEntity(entity, m_stencilFillShader);

        // 1. Fill mask buffer pass
        m_objectsMaskRT->bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderEntity(entity, m_maskFillShader);

        // 2. Jump flood init pass
        // calculate the number of jump flood passes needed for the current outline width
        // + 1.0f to handle half pixel inset of the init pass and antialiasing
        int numSteps      = glm::ceil(glm::log2(outlineWidth + 1.0f));
        int jfaIterations = numSteps - 1;

        // Choose a starting buffer so we always finish on the same buffer (A)
        auto startRT = (jfaIterations % 2 == 0) ? m_jumpFloodBufferB : m_jumpFloodBufferA;

        glDisable(GL_DEPTH_TEST);

        startRT->bind();
        glClear(GL_COLOR_BUFFER_BIT);

        m_objectsMaskRT->bindTexture();
        m_jumpFloodInitPS->bind();
        m_jumpFloodInitPS->render();

        // 3. Jump flood pass
        // TODO: Single axis 
        m_jumpFloodPS->bind();

        for (int i = jfaIterations; i >= 0; --i)
        {
            float stepWidth = glm::pow(2, i);
            m_jumpFloodPS->getShader()->setUniform("step_width", stepWidth);

            // Ping-pong buffers A and B
            if (i % 2 == 1)
            {
                m_jumpFloodBufferB->bind();

                m_jumpFloodBufferA->bindTexture();
                m_jumpFloodPS->render();
            }
            else
            {
                m_jumpFloodBufferA->bind();

                m_jumpFloodBufferB->bindTexture();
                m_jumpFloodPS->render();
            }
        }

        // 4. Draw the outline
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        dstRT->bind();

        m_jumpFloodOutlinePS->bind();
        m_jumpFloodOutlinePS->getShader()->setUniform("outline_color", outlineColor);
        m_jumpFloodOutlinePS->getShader()->setUniform("outline_width", outlineWidth);
        m_jumpFloodBufferA->bindTexture();
        m_jumpFloodOutlinePS->render();

        glEnable(GL_DEPTH_TEST);
        glDisable(GL_STENCIL_TEST);
    }

    void JFAOutline::renderEntity(Entity entity, const ref<Shader> shader)
    {
        // render entity
        auto& smc = entity.getComponent<StaticMeshComponent>();
        auto& tc = entity.getComponent<TransformComponent>();
        auto& mesh = smc.mesh;

        mesh->bind();
        shader->bind();
        shader->updateGlobalUniforms(tc);

        auto& submeshes = mesh->getSubmeshes();
        for (uint32_t submeshIndex = 0; submeshIndex < submeshes.size(); ++submeshIndex)
        {
            mesh->render(submeshIndex);
        }
    }

}