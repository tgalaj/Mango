#include "mgpch.h"

#include "RenderingSystem.h"
#include "Mango/Core/AssetManager.h"
#include "Mango/Rendering/BloomPS.h"
#include "Mango/Rendering/DeferredRendering.h"
#include "Mango/Rendering/Picking.h"
#include "Mango/Rendering/SSAO.h"
#include "Mango/Rendering/ShaderGlobals.h"
#include "Mango/Scene/Components.h"
#include "Mango/Window/Window.h"

namespace mango
{
    bool         RenderingSystem::DEBUG_RENDERING    = false;
    unsigned int RenderingSystem::DEBUG_WINDOW_WIDTH = 0;
    
    RenderingSystem::RenderingSystem()
        : System("RenderingSystem")
    {

    }

    void RenderingSystem::onInit()
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("RenderingSystem::onInit");

        m_statistics.vendorName    = (const char*)glGetString(GL_VENDOR);
        m_statistics.rendererName  = (const char*)glGetString(GL_RENDERER);
        m_statistics.driverVersion = (const char*)glGetString(GL_VERSION);
        m_statistics.glslVersion   = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);

        MG_CORE_ASSERT_MSG(Services::application()              != nullptr, "application can't be nullptr!");
        MG_CORE_ASSERT_MSG(Services::application()->getWindow() != nullptr, "window can't be nullptr!");
        MG_CORE_ASSERT_MSG(Services::eventBus()                 != nullptr, "eventBus can't be nullptr!");

        m_mainWindow = Services::application()->getWindow().get();

        Services::eventBus()->subscribe<EntityRemovedEvent>(MG_BIND_EVENT(RenderingSystem::receive));
        Services::eventBus()->subscribe<ComponentAddedEvent<ModelRendererComponent>>(MG_BIND_EVENT(RenderingSystem::receive));
        Services::eventBus()->subscribe<ComponentReplacedEvent<ModelRendererComponent>>(MG_BIND_EVENT(RenderingSystem::receive));
        Services::eventBus()->subscribe<ComponentRemovedEvent<ModelRendererComponent>>(MG_BIND_EVENT(RenderingSystem::receive));
        Services::eventBus()->subscribe<ActiveSceneChangedEvent>(MG_BIND_EVENT(RenderingSystem::receive));

        AssetManager::createTexture2D1x1("default_diffuse",  glm::uvec4(255, 255, 255, 255));
        AssetManager::createTexture2D1x1("default_specular", glm::uvec4(0,   0,   0,   255));
        AssetManager::createTexture2D1x1("default_normal",   glm::uvec4(128, 127, 254, 255));
        AssetManager::createTexture2D1x1("default_emission", glm::uvec4(0,   0,   0,   255));
        AssetManager::createTexture2D1x1("default_depth",    glm::uvec4(0,   0,   0,   255));

        m_opaqueQueue.reserve(50);
        m_alphaQueue.reserve(5);

        m_forwardAmbient = AssetManager::createShader("Forward-Ambient", "Forward-Light.vert", "Forward-Ambient.frag");
        m_forwardAmbient->link();

        m_forwardDirectional = AssetManager::createShader("Forward-Directional", "Forward-Light.vert", "Forward-Directional.frag");
        m_forwardDirectional->link();

        m_forwardPoint = AssetManager::createShader("Forward-Point", "Forward-Light.vert", "Forward-Point.frag");
        m_forwardPoint->link();

        m_forwardSpot = AssetManager::createShader("Forward-Spot", "Forward-Light.vert", "Forward-Spot.frag");
        m_forwardSpot->link();

        m_debugRendering = AssetManager::createShader("Debug-Rendering", "FSQ.vert", "DebugRendering.frag");
        m_debugRendering->link();

        m_shadowMapGenerator = AssetManager::createShader("Shadow-Map-Gen", "Shadow-Map-Gen.vert", "Shadow-Map-Gen.frag");
        m_shadowMapGenerator->link();

        m_omniShadowMapGenerator = AssetManager::createShader("Omni-Shadow-Map-Gen", "Omni-Shadow-Map-Gen.vert", "Omni-Shadow-Map-Gen.frag", "Omni-Shadow-Map-Gen.geom");
        m_omniShadowMapGenerator->link();

        m_blendingShader = AssetManager::createShader("Blending-Shader", "Blending.vert", "Blending.frag");
        m_blendingShader->link();

        m_enviroMappingShader = AssetManager::createShader("EnviroMapping", "EnviroMapping.vert", "EnviroMapping.frag");
        m_enviroMappingShader->link();

        m_deferredDirectional = AssetManager::createShader("Deferred-Directional", "FSQ.vert", "Deferred-Directional.frag");
        m_deferredDirectional->link();

        m_deferredPoint = AssetManager::createShader("Deferred-Point", "DebugObjectGeometry.vert", "Deferred-Point.frag");
        m_deferredPoint->link();

        m_deferredSpot = AssetManager::createShader("Deferred-Spot", "DebugObjectGeometry.vert", "Deferred-Spot.frag");
        m_deferredSpot->link();

        m_boundingboxShader = AssetManager::createShader("DebugRenderBB", "DebugObjectGeometry.vert", "DebugObjectGeometry.frag");
        m_boundingboxShader->link();

        m_nullShader = AssetManager::createShader("NullShader", "DebugObjectGeometry.vert", "Shadow-Map-Gen.frag");
        m_nullShader->link();

        m_lightBoundingSphere = Model();
        m_lightBoundingSphere.genSphere(1.1f, 12);

        m_lightBoundingCone = Model();
        m_lightBoundingCone.genCone(1.1f, 1.1f, 12, 1);

        int width  = m_mainWindow->getWidth();
        int height = m_mainWindow->getHeight();

        DEBUG_WINDOW_WIDTH = GLuint(width / 5.0f);
        sceneAmbientColor = glm::vec3(0.18f);

        m_hdrFilter = std::make_shared<PostprocessEffect>();
        m_hdrFilter->init("HDR_PS", "HDR_PS.frag");

        m_fxaaFilter = std::make_shared<PostprocessEffect>();
        m_fxaaFilter->init("FXAA_PS", "FXAA_PS.frag");

        m_deferredRendering = std::make_shared<DeferredRendering>();
        m_deferredRendering->init();
        m_deferredRendering->createGBuffer(width, height);

        m_gbufferShader = AssetManager::getShader("GBuffer");

        m_bloomFilter = std::make_shared<BloomPS>();
        m_bloomFilter->init("Bloom_PS", "Bloom_PS.frag");
        m_bloomFilter->create(width, height);

        m_ssao = std::make_shared<SSAO>();
        m_ssao->init("SSAO_PS", "SSAO.frag");
        m_ssao->create(width, height);

        m_picking = std::make_shared<Picking>();
        m_picking->init(width, height);

        m_mainRenderTarget = std::make_shared<RenderTarget>();
        m_mainRenderTarget->create(width, height, RenderTarget::ColorInternalFormat::RGBA16F, RenderTarget::DepthInternalFormat::DEPTH32F_STENCIL8);

        m_helperRenderTarget = std::make_shared<RenderTarget>();
        m_helperRenderTarget->create(width, height, RenderTarget::ColorInternalFormat::RGBA16F, RenderTarget::DepthInternalFormat::DEPTH32F_STENCIL8);

        m_dirShadowMap = std::make_shared<RenderTarget>();
        m_dirShadowMap->create(2048, 2048, RenderTarget::DepthInternalFormat::DEPTH24);

        m_spotShadowMap = std::make_shared<RenderTarget>();
        m_spotShadowMap->create(1024, 1024, RenderTarget::DepthInternalFormat::DEPTH24);

        m_omniShadowMap = std::make_shared<RenderTarget>();
        m_omniShadowMap->create(512, 512, RenderTarget::DepthInternalFormat::DEPTH24, RenderTarget::RenderTargetType::TexCube);

        initRenderingStates();
    }

    void RenderingSystem::onUpdate(float dt)
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("RenderingSystem::onUpdate");
        MG_CORE_ASSERT_MSG(m_activeScene != nullptr, "Active scene can't be nullptr!");

        glBindFramebuffer(GL_FRAMEBUFFER, m_outputToOffscreenTexture ? m_mainRenderTarget->m_fbo : 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // TODO: create two methods: renderGame and renderEditor + use switch
        if (m_mode == RenderingMode::GAME)
        {
            auto primaryCameraEntity = m_activeScene->getPrimaryCamera();

            if (!primaryCameraEntity) return;

            auto& cc = primaryCameraEntity.getComponent<CameraComponent>();
            auto& tc = primaryCameraEntity.getComponent<TransformComponent>();
            
            m_camera         = &cc.camera;
            m_cameraPosition = tc.getPosition();

            // Update the view matrix of the primary camera
            glm::mat4 cameraRotation    = glm::mat4_cast(tc.getOrientation());
            glm::mat4 cameraTranslation = glm::translate(glm::mat4(1.0f), -tc.getPosition());

            m_camera->setView(cameraRotation * cameraTranslation);

            //renderForward(m_activeScene);
            renderDeferred(m_activeScene);
        }

        if (m_mode == RenderingMode::EDITOR)
        {
            renderDeferred(m_activeScene);

            if (DEBUG_RENDERING)
            {
                renderDebugLightsBoundingBoxes(m_activeScene);
                renderDebug();
            }
        }
    }

    void RenderingSystem::onDestroy()
    {
        MG_PROFILE_ZONE_SCOPED;

        m_opaqueQueue.clear();
        m_alphaQueue.clear();
        m_enviroStaticQueue.clear();
        m_enviroDynamicQueue.clear();
    }

    void RenderingSystem::receive(const EntityRemovedEvent& event)
    {
        MG_PROFILE_ZONE_SCOPED;

        if (event.entity.get().hasComponent<ModelRendererComponent>())
        {
            auto renderQueue = event.entity.get().getComponent<ModelRendererComponent>().getRenderQueue();
            removeEntityFromRenderQueue(event.entity, renderQueue);
        }
    }

    void RenderingSystem::receive(const ComponentAddedEvent<ModelRendererComponent>& event)
    {
        MG_PROFILE_ZONE_SCOPED;
        addEntityToRenderQueue(event.entity, event.component.getRenderQueue());
    }

    void RenderingSystem::receive(const ComponentReplacedEvent<ModelRendererComponent>& event)
    {
        MG_PROFILE_ZONE_SCOPED;
        removeEntityFromRenderQueue(event.entity, event.component.getRenderQueue());
        addEntityToRenderQueue(event.entity, event.component.getRenderQueue());
    }

    void RenderingSystem::receive(const ComponentRemovedEvent<ModelRendererComponent>& event)
    {
        MG_PROFILE_ZONE_SCOPED;
        removeEntityFromRenderQueue(event.entity, event.component.getRenderQueue());
    }

    void RenderingSystem::receive(const ActiveSceneChangedEvent& event)
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_CORE_ASSERT_MSG(event.scene != nullptr, "New active scene pointer is nullptr.");

        m_opaqueQueue.clear();
        m_alphaQueue.clear();
        m_enviroStaticQueue.clear();
        m_enviroDynamicQueue.clear();

        m_activeScene = event.scene;

        auto view = m_activeScene->getEntitiesWithComponent<ModelRendererComponent>();
        for (auto e : view)
        {
            Entity entity = { e, m_activeScene };
            auto& mrc = entity.getComponent<ModelRendererComponent>();

            addEntityToRenderQueue(entity, mrc.getRenderQueue());
        }
    }

    void RenderingSystem::setSkybox(const std::shared_ptr<Skybox>& skybox)
    {
        m_skybox = skybox;
    }

    void RenderingSystem::resize(unsigned width, unsigned height)
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("RenderingSystem::resize");

        m_mainFramebufferSize = { width, height };

        m_mainRenderTarget->clear();
        m_helperRenderTarget->clear();
        m_deferredRendering->clearGBuffer();
        m_bloomFilter->clear();
        m_ssao->clear();

        m_mainRenderTarget->create(width, height, RenderTarget::ColorInternalFormat::RGBA16F, RenderTarget::DepthInternalFormat::DEPTH32F_STENCIL8);
        m_helperRenderTarget->create(width, height, RenderTarget::ColorInternalFormat::RGBA16F, RenderTarget::DepthInternalFormat::DEPTH32F_STENCIL8);
        m_deferredRendering->createGBuffer(width, height);
        m_bloomFilter->create(width, height);
        m_ssao->create(width, height);
        m_picking->resize(width, height);

        DEBUG_WINDOW_WIDTH = GLuint(width / 5.0f);

        if (m_camera)
        {
            m_camera->resize(width, height);
        }
    }

    int RenderingSystem::getSelectedEntityID(int mouseX, int mouseY)
    {
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_SCISSOR_TEST);
        glClear(GL_DEPTH_BUFFER_BIT);

        m_picking->bindFramebuffer();
        m_picking->setPickingRegion(mouseX, mouseY);

        auto pickingShader = m_picking->getShader();
        pickingShader->bind();

        auto view = m_activeScene->getEntitiesWithComponent<TransformComponent, ModelRendererComponent>();
        for (auto entity : view)
        {
            auto [tc, mrc] = view.get<TransformComponent, ModelRendererComponent>(entity);

            pickingShader->updateGlobalUniforms(tc);
            int id = (int)entity;
            pickingShader->setUniform("objectID", (int)id);
            mrc.model.render(*pickingShader);
        }
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_SCISSOR_TEST);

        m_picking->setPickingRegion(0, 0, m_mainRenderTarget->getWidth(), m_mainRenderTarget->getHeight());

        return m_picking->getPickedID();
    }

    uint32_t RenderingSystem::getOutputOffscreenTextureID() const
    {
        return m_mainRenderTarget->m_textureID[0];
    }

    void RenderingSystem::setRenderingMode(RenderingMode mode, Camera* editorCamera /*= nullptr*/, const glm::vec3& editorCameraPosition /*= glm::vec3(0.0f)*/)
    {
        m_mode           = mode;
        m_camera         = editorCamera;
        m_cameraPosition = editorCameraPosition;
    }

    Camera& RenderingSystem::getCamera() const
    {
        return *(m_camera);
    }

    void RenderingSystem::initRenderingStates()
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("RenderingSystem::initRenderingStates");

        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);

        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);

        glClearColor(0, 0, 0, 1);
    }

    void RenderingSystem::beginForwardRendering()
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("RenderingSystem::beginForwardRendering");

        glBlendFunc(GL_ONE, GL_ONE);
        glDepthMask(GL_FALSE);
        glDepthFunc(GL_EQUAL);
    }

    void RenderingSystem::endForwardRendering()
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("RenderingSystem::endForwardRendering");

        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
    }

    void RenderingSystem::bindMainRenderTarget()
    {
        MG_PROFILE_ZONE_SCOPED;
        m_mainRenderTarget->bind();
    }

    void RenderingSystem::applyPostprocess(std::shared_ptr<PostprocessEffect> & effect, 
                                           std::shared_ptr<RenderTarget>      * src, 
                                           std::shared_ptr<RenderTarget>      * dst)
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("RenderingSystem::applyPostprocess");

        if(dst == nullptr)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, m_mainWindow->getWidth(), m_mainWindow->getHeight());
        }
        else
        {
            dst->get()->bind();
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        effect->bind();
        src->get()->bindTexture();
        effect->render();
    }

    void RenderingSystem::renderForward(Scene* scene)
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("RenderingSystem::renderForward");

        /* Render everything to offscreen FBO */
        m_mainRenderTarget->bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_forwardAmbient->bind();
        m_forwardAmbient->setUniform("s_scene_ambient", sceneAmbientColor);
        renderOpaque(m_forwardAmbient);

        renderLightsForward(scene);

        /* Sort transparent objects back to front */
        sortAlpha();

        /* Render transparent objects */
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_CULL_FACE);
        m_blendingShader->bind();
        renderAlpha(m_blendingShader);
        glEnable(GL_CULL_FACE);

        if (DEBUG_RENDERING)
        {
            renderDebugLightsBoundingBoxes(scene);
        }

        /* Render skybox */
        if (m_skybox != nullptr)
        {
            m_skybox->render(getCamera().getProjection(), getCamera().getView());

            m_enviroMappingShader->bind();
            m_enviroMappingShader->setSubroutine(Shader::Type::FRAGMENT, "reflection"); // TODO: control this using Material class

            m_skybox->bindSkyboxTexture();
            renderEnviroMappingStatic(m_enviroMappingShader);
        }

        /* Apply postprocess effect */
        m_bloomFilter->extractBrightness(m_mainRenderTarget, 1.0);
        m_bloomFilter->blurGaussian(2);
        m_bloomFilter->bindBrightnessTexture(1);

        applyPostprocess(m_hdrFilter, &m_mainRenderTarget, &m_helperRenderTarget);
        applyPostprocess(m_fxaaFilter, &m_helperRenderTarget, m_outputToOffscreenTexture ? &m_mainRenderTarget : 0);
    }

    void RenderingSystem::renderDeferred(Scene* scene)
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("RenderingSystem::renderDeferred");

        /* Geometry Pass - Render data to GBuffer */
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);

        m_deferredRendering->bindGBuffer();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_gbufferShader->bind();
        renderOpaque(m_gbufferShader);

        /* Compute SSAO */
        m_ssao->computeSSAO(m_deferredRendering, getCamera().getView(), getCamera().getProjection());
        m_ssao->blurSSAO();

        /* Light Pass - compute lighting */
        glDepthMask(GL_FALSE);
        glDisable(GL_DEPTH_TEST);

        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_ONE, GL_ONE);

        m_mainRenderTarget->bind();
        glClear(GL_COLOR_BUFFER_BIT);

        m_ssao->bindBlurredSSAOTexture(4); //TODO: replace magic number with a variable
        renderLightsDeferred(scene);

        m_deferredRendering->bindGBufferReadOnly();
        m_mainRenderTarget->bindWriteOnly();
        glBlitFramebuffer(0, 0, m_mainWindow->getWidth(), m_mainWindow->getHeight(),
                          0, 0, m_mainWindow->getWidth(), m_mainWindow->getHeight(),
                          GL_DEPTH_BUFFER_BIT, GL_NEAREST);

        m_mainRenderTarget->bind();
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);

        if (DEBUG_RENDERING && m_mode == RenderingMode::EDITOR)
        {
            renderDebugLightsBoundingBoxes(scene);
        }

        /* Sort transparent objects back to front */
        sortAlpha();

        /* Render transparent objects */
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_CULL_FACE);
        m_blendingShader->bind();
        renderAlpha(m_blendingShader);
        glEnable(GL_CULL_FACE);

        /* Render skybox */
        if (m_skybox != nullptr)
        {
            m_skybox->render(getCamera().getProjection(), getCamera().getView());

            m_enviroMappingShader->bind();
            //m_enviro_mapping_shader->setSubroutine(Shader::Type::FRAGMENT, "refraction"); // TODO: control this using Material class
            m_enviroMappingShader->setSubroutine(Shader::Type::FRAGMENT, "reflection");

            m_skybox->bindSkyboxTexture();
            renderEnviroMappingStatic(m_enviroMappingShader);
        }

        /* Apply postprocess effect */
        m_bloomFilter->extractBrightness(m_mainRenderTarget, 1.0);
        m_bloomFilter->blurGaussian(2);
        m_bloomFilter->bindBrightnessTexture(1);

        applyPostprocess(m_hdrFilter, &m_mainRenderTarget, &m_helperRenderTarget);
        applyPostprocess(m_fxaaFilter, &m_helperRenderTarget, m_outputToOffscreenTexture ? &m_mainRenderTarget : 0);
    }

    void RenderingSystem::renderDebug()
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("RenderingSystem::renderDebug");

        glDisable(GL_BLEND);
        glClear(GL_DEPTH_BUFFER_BIT);

        m_debugRendering->bind();

        m_debugRendering->setSubroutine(Shader::Type::FRAGMENT, "debugColorTarget");
        m_deferredRendering->bindGBufferTexture(0, (GLuint)DeferredRendering::GBufferPropertyName::POSITION);
        glViewport(DEBUG_WINDOW_WIDTH * 0, 0, DEBUG_WINDOW_WIDTH, DEBUG_WINDOW_WIDTH / m_mainWindow->getAspectRatio());
        m_deferredRendering->render();

        m_deferredRendering->bindGBufferTexture(0, (GLuint)DeferredRendering::GBufferPropertyName::ALBEDO_SPECULAR);
        glViewport(DEBUG_WINDOW_WIDTH * 1, 0, DEBUG_WINDOW_WIDTH, DEBUG_WINDOW_WIDTH / m_mainWindow->getAspectRatio());
        m_deferredRendering->render();

        m_deferredRendering->bindGBufferTexture(0, (GLuint)DeferredRendering::GBufferPropertyName::NORMAL);
        glViewport(DEBUG_WINDOW_WIDTH * 2, 0, DEBUG_WINDOW_WIDTH, DEBUG_WINDOW_WIDTH / m_mainWindow->getAspectRatio());
        m_deferredRendering->render();

        m_debugRendering->setSubroutine(Shader::Type::FRAGMENT, "debugDepthTarget");
        m_deferredRendering->bindGBufferTexture(0, (GLuint)DeferredRendering::GBufferPropertyName::DEPTH);
        glViewport(DEBUG_WINDOW_WIDTH * 3, 0, DEBUG_WINDOW_WIDTH, DEBUG_WINDOW_WIDTH / m_mainWindow->getAspectRatio());
        m_deferredRendering->render();

        glEnable(GL_BLEND);
    }

    void RenderingSystem::renderDebugLightsBoundingBoxes(Scene* scene)
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("RenderingSystem::renderDebugLightsBoundingBoxes");

        glDisable(GL_BLEND);

        /* Point Lights */
        {
            m_lightBoundingSphere.setDrawMode(GL_LINES);
            auto view = scene->getEntitiesWithComponent<PointLightComponent, TransformComponent>();
            for (auto entity : view)
            {
                auto [pointLight, transform] = view.get(entity);

                auto model        = glm::translate(glm::mat4(1.0f), transform.getPosition()) *
                                    glm::scale(glm::mat4(1.0f), glm::vec3(pointLight.getRange()));
                auto& view        = getCamera().getView();
                auto& projection  = getCamera().getProjection();

                m_boundingboxShader->bind();
                m_boundingboxShader->setUniform("g_mvp", projection * view * model);
                m_boundingboxShader->setUniform("color", glm::vec4(1.0f, 1.0, 1.0, 1.0f));

                m_lightBoundingSphere.render(*m_boundingboxShader);
            }
            m_lightBoundingSphere.setDrawMode(GL_TRIANGLES);
        }

        /* Spot Lights */
        {
            m_lightBoundingCone.setDrawMode(GL_LINES);
            auto view = scene->getEntitiesWithComponent<SpotLightComponent, TransformComponent>();
            for (auto entity : view)
            {
                auto [spotLight, transform] = view.get(entity);

                float heightScale = spotLight.getRange();
                float radiusScale = spotLight.getRange() * glm::tan(spotLight.getCutOffAngle());

                auto model      = glm::translate(glm::mat4(1.0f), transform.getPosition()) *
                                  glm::mat4_cast(glm::inverse(transform.getOrientation()) * glm::angleAxis(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f))) *
                                  glm::scale(glm::mat4(1.0f), glm::vec3(radiusScale, heightScale, radiusScale));
                auto view       = getCamera().getView();
                auto projection = getCamera().getProjection();

                m_boundingboxShader->bind();
                m_boundingboxShader->setUniform("g_mvp", projection * view * model);
                m_boundingboxShader->setUniform("color", glm::vec4(1.0f, 0.0, 0.0, 1.0f));

                m_lightBoundingCone.render(*m_boundingboxShader);
            }
            m_lightBoundingCone.setDrawMode(GL_TRIANGLES);
        }
        glEnable(GL_BLEND);
    }

    void RenderingSystem::renderOpaque(const std::shared_ptr<Shader> & shader)
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("RenderingSystem::renderOpaque");

        for (auto& entity : m_opaqueQueue)
        {
            auto& modelRenderer = entity.getComponent<ModelRendererComponent>();
            auto& transform     = entity.getComponent<TransformComponent>();

            shader->updateGlobalUniforms(transform);
            modelRenderer.model.render(*shader);
        }
    }

    void RenderingSystem::renderAlpha(const std::shared_ptr<Shader>& shader)
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("RenderingSystem::renderAlpha");

        for (auto& entity : m_alphaQueue)
        {
            auto& modelRenderer = entity.getComponent<ModelRendererComponent>();
            auto& transform     = entity.getComponent<TransformComponent>();

            shader->updateGlobalUniforms(transform);
            modelRenderer.model.render(*shader);
        }
    }

    void RenderingSystem::renderEnviroMappingStatic(const std::shared_ptr<Shader>& shader)
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("RenderingSystem::renderEnviroMappingStatic");

        for (auto& entity : m_enviroStaticQueue)
        {
            auto& modelRenderer = entity.getComponent<ModelRendererComponent>();
            auto& transform     = entity.getComponent<TransformComponent>();

            shader->updateGlobalUniforms(transform);
            modelRenderer.model.render(*shader);
        }
    }

    void RenderingSystem::renderDynamicEnviroMapping(const std::shared_ptr<Shader>& shader)
    {

    }

    void RenderingSystem::renderLightsForward(Scene* scene)
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("RenderingSystem::renderLightsForward");

        /*
         * TODO:
         * Transform shadow info into light component;
         * Sort lights based on the parameter if light casts shadows or not;
         * Then render lights with shadows and then lights without shadows;
         */

        /* Directional Lights */
        {
            MG_PROFILE_ZONE_NAMED_N(dirLightsZone, "Forward Directional Lights", true);

            auto view = scene->getEntitiesWithComponent<DirectionalLightComponent, TransformComponent>();
            for(auto entity : view)
            {
                auto [directionalLight, transform] = view.get(entity);
                ShadowInfo shadowInfo = directionalLight.getShadowInfo();
                glm::mat4 lightMatrix = glm::mat4(0.0f);

                if(shadowInfo.getCastsShadows())
                {
                    m_shadowMapGenerator->bind();

                    m_dirShadowMap->bind();
                    glClear(GL_DEPTH_BUFFER_BIT);

                    lightMatrix = shadowInfo.getProjection() * glm::lookAt(-transform.getDirection(), glm::vec3(0.0f), glm::vec3(0, 1, 0));
                    m_shadowMapGenerator->setUniform("s_light_matrix", lightMatrix);

                    glCullFace(GL_FRONT);
                    renderOpaque(m_shadowMapGenerator);
                    renderEnviroMappingStatic(m_shadowMapGenerator);
                    glCullFace(GL_BACK);
                }

                bindMainRenderTarget();
            
                m_forwardDirectional->bind();
                m_dirShadowMap->bindTexture(SHADOW_MAP);

                m_forwardDirectional->setUniform(S_DIRECTIONAL_LIGHT ".base.color",     directionalLight.color);
                m_forwardDirectional->setUniform(S_DIRECTIONAL_LIGHT ".base.intensity", directionalLight.intensity);
                m_forwardDirectional->setUniform(S_DIRECTIONAL_LIGHT ".direction",      transform.getDirection());
                m_forwardDirectional->setUniform("s_light_matrix", lightMatrix);

                beginForwardRendering();
                renderOpaque(m_forwardDirectional);
                endForwardRendering();
            }
        }

        /* Point Lights */
        {
            MG_PROFILE_ZONE_NAMED_N(dirLightsZone, "Forward Point Lights", true);

            auto view = scene->getEntitiesWithComponent<PointLightComponent, TransformComponent>();
            for (auto entity : view)
            {
                auto [pointLight, transform] = view.get(entity);

                ShadowInfo shadowInfo = pointLight.getShadowInfo();
                glm::mat4 lightMatrices[6];

                if (shadowInfo.getCastsShadows())
                {
                    m_omniShadowMapGenerator->bind();

                    m_omniShadowMap->bind();
                    glClear(GL_DEPTH_BUFFER_BIT);

                    lightMatrices[0] = shadowInfo.getProjection() * glm::lookAt(transform.getPosition(), transform.getPosition() + glm::vec3( 1,  0,  0), glm::vec3(0, -1,  0));
                    lightMatrices[1] = shadowInfo.getProjection() * glm::lookAt(transform.getPosition(), transform.getPosition() + glm::vec3(-1,  0,  0), glm::vec3(0, -1,  0));
                    lightMatrices[2] = shadowInfo.getProjection() * glm::lookAt(transform.getPosition(), transform.getPosition() + glm::vec3( 0,  1,  0), glm::vec3(0,  0,  1));
                    lightMatrices[3] = shadowInfo.getProjection() * glm::lookAt(transform.getPosition(), transform.getPosition() + glm::vec3( 0, -1,  0), glm::vec3(0,  0, -1));
                    lightMatrices[4] = shadowInfo.getProjection() * glm::lookAt(transform.getPosition(), transform.getPosition() + glm::vec3( 0,  0,  1), glm::vec3(0, -1,  0));
                    lightMatrices[5] = shadowInfo.getProjection() * glm::lookAt(transform.getPosition(), transform.getPosition() + glm::vec3( 0,  0, -1), glm::vec3(0, -1,  0));

                    m_omniShadowMapGenerator->setUniform("s_light_matrices", lightMatrices, 6);
                    m_omniShadowMapGenerator->setUniform("s_light_pos",      transform.getPosition());
                    m_omniShadowMapGenerator->setUniform("s_far_plane",      100.0f);

                    glCullFace(GL_FRONT);
                    renderOpaque(m_omniShadowMapGenerator);
                    renderEnviroMappingStatic(m_omniShadowMapGenerator);
                    glCullFace(GL_BACK);
                }

                bindMainRenderTarget();

                m_forwardPoint->bind();
                m_omniShadowMap->bindTexture(SHADOW_MAP);

                m_forwardPoint->setUniform(S_POINT_LIGHT ".base.color",      pointLight.color);
                m_forwardPoint->setUniform(S_POINT_LIGHT ".base.intensity",  pointLight.intensity);
                m_forwardPoint->setUniform(S_POINT_LIGHT ".atten.constant",  pointLight.getAttenuation().constant);
                m_forwardPoint->setUniform(S_POINT_LIGHT ".atten.linear",    pointLight.getAttenuation().linear);
                m_forwardPoint->setUniform(S_POINT_LIGHT ".atten.quadratic", pointLight.getAttenuation().quadratic);
                m_forwardPoint->setUniform(S_POINT_LIGHT ".position",        transform.getPosition());
                m_forwardPoint->setUniform(S_POINT_LIGHT ".range",           pointLight.getRange());
                m_forwardPoint->setUniform("s_far_plane", 100.0f);

                beginForwardRendering();
                renderOpaque(m_forwardPoint);
                endForwardRendering();
            }
        }
        /* Spot Lights */
        {
            MG_PROFILE_ZONE_NAMED_N(dirLightsZone, "Forward Spot Lights", true);

            auto view = scene->getEntitiesWithComponent<SpotLightComponent, TransformComponent>();
            for (auto entity : view)
            {
                auto [spotLight, transform] = view.get(entity);

                ShadowInfo shadowInfo = spotLight.getShadowInfo();
                glm::mat4 lightMatrix = glm::mat4(0.0f);

                if (shadowInfo.getCastsShadows())
                {
                    m_shadowMapGenerator->bind();

                    m_spotShadowMap->bind();
                    glClear(GL_DEPTH_BUFFER_BIT);

                    lightMatrix = shadowInfo.getProjection() * glm::lookAt(transform.getPosition(), transform.getPosition() + transform.getDirection(), glm::vec3(0, 1, 0));
                    m_shadowMapGenerator->setUniform("s_light_matrix", lightMatrix);

                    glCullFace(GL_FRONT);
                    renderOpaque(m_shadowMapGenerator);
                    renderEnviroMappingStatic(m_shadowMapGenerator);
                    glCullFace(GL_BACK);
                }

                bindMainRenderTarget();

                m_forwardSpot->bind();
                m_spotShadowMap->bindTexture(SHADOW_MAP);

                m_forwardSpot->setUniform(S_SPOT_LIGHT ".point.base.color",      spotLight.color);
                m_forwardSpot->setUniform(S_SPOT_LIGHT ".point.base.intensity",  spotLight.intensity);
                m_forwardSpot->setUniform(S_SPOT_LIGHT ".point.atten.constant",  spotLight.getAttenuation().constant);
                m_forwardSpot->setUniform(S_SPOT_LIGHT ".point.atten.linear",    spotLight.getAttenuation().linear);
                m_forwardSpot->setUniform(S_SPOT_LIGHT ".point.atten.quadratic", spotLight.getAttenuation().quadratic);
                m_forwardSpot->setUniform(S_SPOT_LIGHT ".point.position",        transform.getPosition());
                m_forwardSpot->setUniform(S_SPOT_LIGHT ".point.range",           spotLight.getRange());
                m_forwardSpot->setUniform(S_SPOT_LIGHT ".direction",             transform.getDirection());
                m_forwardSpot->setUniform(S_SPOT_LIGHT ".cutoff",                glm::cos(spotLight.getCutOffAngle()));
                m_forwardSpot->setUniform("s_light_matrix",                      lightMatrix);

                beginForwardRendering();
                renderOpaque(m_forwardSpot);
                endForwardRendering();
            }
        }
    }

    void RenderingSystem::renderLightsDeferred(Scene* scene)
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("RenderingSystem::renderLightsDeferred");

        /* Directional Lights */
        {
            MG_PROFILE_ZONE_NAMED_N(dirLightsZone, "Deferred Directional Lights", true);
            MG_PROFILE_GL_ZONE("Deferred Directional Lights");

            auto view = scene->getEntitiesWithComponent<DirectionalLightComponent, TransformComponent>();
            for (auto entity : view)
            {
                auto [directionalLight, transform] = view.get(entity);

                ShadowInfo shadowInfo = directionalLight.getShadowInfo();
                glm::mat4 lightMatrix = glm::mat4(0.0f);

                if (shadowInfo.getCastsShadows())
                {
                    glEnable(GL_DEPTH_TEST);
                    glDepthMask(GL_TRUE);

                    m_shadowMapGenerator->bind();
                    m_dirShadowMap->bind();
                    glClear(GL_DEPTH_BUFFER_BIT);

                    lightMatrix = shadowInfo.getProjection() * glm::lookAt(-transform.getDirection(), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                    m_shadowMapGenerator->setUniform("s_light_matrix", lightMatrix);

                    glCullFace(GL_FRONT);
                    renderOpaque(m_shadowMapGenerator);
                    renderEnviroMappingStatic(m_shadowMapGenerator);
                    glCullFace(GL_BACK);

                    glDepthMask(GL_FALSE);
                    glDisable(GL_DEPTH_TEST);
                }

                bindMainRenderTarget();

                m_deferredDirectional->bind();
                m_deferredRendering->bindGBufferTextures();
                m_dirShadowMap->bindTexture(SHADOW_MAP);

                m_deferredDirectional->setUniform("s_scene_ambient", sceneAmbientColor);
                m_deferredDirectional->setUniform(S_DIRECTIONAL_LIGHT ".base.color",     directionalLight.color);
                m_deferredDirectional->setUniform(S_DIRECTIONAL_LIGHT ".base.intensity", directionalLight.intensity);
                m_deferredDirectional->setUniform(S_DIRECTIONAL_LIGHT ".direction",      transform.getDirection());
                m_deferredDirectional->setUniform("s_light_matrix", lightMatrix);

                m_deferredRendering->render();
            }
        }

        /* Point Lights */
        glEnable(GL_STENCIL_TEST);
        {   
            MG_PROFILE_ZONE_NAMED_N(pointLightsZone, "Deferred Point Lights", true);
            MG_PROFILE_GL_ZONE("Deferred Point Lights");

            auto view = scene->getEntitiesWithComponent<PointLightComponent, TransformComponent>();
            for (auto entity : view)
            {
                auto [pointLight, transform] = view.get(entity);

                ShadowInfo shadowInfo = pointLight.getShadowInfo();

                if (shadowInfo.getCastsShadows())
                {
                    glEnable(GL_DEPTH_TEST);
                    glDepthMask(GL_TRUE);

                    m_omniShadowMapGenerator->bind();

                    m_omniShadowMap->bind();
                    glClear(GL_DEPTH_BUFFER_BIT);
                    
                    glm::mat4 lightMatrices[6]{};
                    lightMatrices[0] = shadowInfo.getProjection() * glm::lookAt(transform.getPosition(), transform.getPosition() + glm::vec3( 1,  0,  0), glm::vec3(0, -1,  0));
                    lightMatrices[1] = shadowInfo.getProjection() * glm::lookAt(transform.getPosition(), transform.getPosition() + glm::vec3(-1,  0,  0), glm::vec3(0, -1,  0));
                    lightMatrices[2] = shadowInfo.getProjection() * glm::lookAt(transform.getPosition(), transform.getPosition() + glm::vec3( 0,  1,  0), glm::vec3(0,  0,  1));
                    lightMatrices[3] = shadowInfo.getProjection() * glm::lookAt(transform.getPosition(), transform.getPosition() + glm::vec3( 0, -1,  0), glm::vec3(0,  0, -1));
                    lightMatrices[4] = shadowInfo.getProjection() * glm::lookAt(transform.getPosition(), transform.getPosition() + glm::vec3( 0,  0,  1), glm::vec3(0, -1,  0));
                    lightMatrices[5] = shadowInfo.getProjection() * glm::lookAt(transform.getPosition(), transform.getPosition() + glm::vec3( 0,  0, -1), glm::vec3(0, -1,  0));

                    m_omniShadowMapGenerator->setUniform("s_light_matrices", lightMatrices, 6);
                    m_omniShadowMapGenerator->setUniform("s_light_pos", transform.getPosition());
                    m_omniShadowMapGenerator->setUniform("s_far_plane", 100.0f);

                    glCullFace(GL_FRONT);
                    renderOpaque(m_omniShadowMapGenerator);
                    renderEnviroMappingStatic(m_omniShadowMapGenerator);
                    glCullFace(GL_BACK);

                    glDepthMask(GL_FALSE);
                    glDisable(GL_DEPTH_TEST);
                }

                bindMainRenderTarget();

                /* Bounding sphere MVP matrix setup */
                auto model = glm::translate(glm::mat4(1.0f), transform.getPosition()) *
                             glm::scale    (glm::mat4(1.0f), glm::vec3(pointLight.getRange()));

                auto& view       = getCamera().getView();
                auto& projection = getCamera().getProjection();
                auto  mvp        = projection * view * model;

                /* Stencil pass */
                m_nullShader->bind();
                glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

                glEnable(GL_DEPTH_TEST);
                glDisable(GL_CULL_FACE);

                glClear(GL_STENCIL_BUFFER_BIT);
                glStencilFunc(GL_ALWAYS, 0, 0);
                glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
                glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

                m_nullShader->setUniform("g_mvp", mvp);
                m_lightBoundingSphere.render(*m_nullShader);

                glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

                /* Lighting pass */
                glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
                glDisable(GL_DEPTH_TEST);
                glEnable(GL_BLEND);
                glBlendEquation(GL_FUNC_ADD);
                glBlendFunc(GL_ONE, GL_ONE);

                glEnable(GL_CULL_FACE);
                glCullFace(GL_FRONT);

                m_deferredPoint->bind();
                m_deferredRendering->bindGBufferTextures();
                m_omniShadowMap->bindTexture(SHADOW_MAP);

                m_deferredPoint->setUniform(S_POINT_LIGHT ".base.color",      pointLight.color);
                m_deferredPoint->setUniform(S_POINT_LIGHT ".base.intensity",  pointLight.intensity);
                m_deferredPoint->setUniform(S_POINT_LIGHT ".atten.constant",  pointLight.getAttenuation().constant);
                m_deferredPoint->setUniform(S_POINT_LIGHT ".atten.linear",    pointLight.getAttenuation().linear);
                m_deferredPoint->setUniform(S_POINT_LIGHT ".atten.quadratic", pointLight.getAttenuation().quadratic);
                m_deferredPoint->setUniform(S_POINT_LIGHT ".position",        transform.getPosition());
                m_deferredPoint->setUniform(S_POINT_LIGHT ".range",           pointLight.getRange());
                m_deferredPoint->setUniform("s_far_plane",                    pointLight.getShadowFarPlane());

                m_deferredPoint->setUniform("g_mvp", mvp);
                m_lightBoundingSphere.render(*m_deferredPoint);

                glCullFace(GL_BACK);
                glDisable(GL_BLEND);
            }
        }

        /* Spot Lights */
        {
            MG_PROFILE_ZONE_NAMED_N(spotLightsZone, "Deferred Spot Lights", true);
            MG_PROFILE_GL_ZONE("Deferred Spot Lights");

            auto view = scene->getEntitiesWithComponent<SpotLightComponent, TransformComponent>();
            for (auto entity : view)
            {
                auto [spotLight, transform] = view.get(entity);

                ShadowInfo shadowInfo = spotLight.getShadowInfo();
                glm::mat4 lightMatrix = glm::mat4(0.0f);

                if (shadowInfo.getCastsShadows())
                {
                    glEnable(GL_DEPTH_TEST);
                    glDepthMask(GL_TRUE);

                    m_shadowMapGenerator->bind();

                    m_spotShadowMap->bind();
                    glClear(GL_DEPTH_BUFFER_BIT);

                    lightMatrix = shadowInfo.getProjection() * glm::lookAt(transform.getPosition(), transform.getPosition() + transform.getDirection(), glm::vec3(0, 1, 0));
                    m_shadowMapGenerator->setUniform("s_light_matrix", lightMatrix);

                    glCullFace(GL_FRONT);
                    renderOpaque(m_shadowMapGenerator);
                    renderEnviroMappingStatic(m_shadowMapGenerator);
                    glCullFace(GL_BACK);

                    glDepthMask(GL_FALSE);
                    glDisable(GL_DEPTH_TEST);
                }

                bindMainRenderTarget();

                /* Bounding cone MVP matrix setup */
                float heightScale = spotLight.getRange();
                float radiusScale = spotLight.getRange() * glm::tan(spotLight.getCutOffAngle());

                auto model = glm::translate(glm::mat4(1.0f), transform.getPosition()) *
                             glm::mat4_cast(glm::inverse(transform.getOrientation()) * glm::angleAxis(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f))) *
                             glm::scale    (glm::mat4(1.0f), glm::vec3(radiusScale, heightScale, radiusScale));

                auto view       = getCamera().getView();
                auto projection = getCamera().getProjection();
                auto mvp        = projection * view * model;

                /* Stencil pass */
                m_nullShader->bind();
                glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

                glEnable(GL_DEPTH_TEST);
                glDisable(GL_CULL_FACE);

                glClear(GL_STENCIL_BUFFER_BIT);
                glStencilFunc(GL_ALWAYS, 0, 0);
                glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
                glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

                m_nullShader->setUniform("g_mvp", mvp);
                m_lightBoundingCone.render(*m_nullShader);

                glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

                /* Lighting pass */
                glStencilFunc(GL_NOTEQUAL, 0, 0xFF);
                glDisable(GL_DEPTH_TEST);
                glEnable(GL_BLEND);
                glBlendEquation(GL_FUNC_ADD);
                glBlendFunc(GL_ONE, GL_ONE);

                glEnable(GL_CULL_FACE);
                glCullFace(GL_FRONT);

                m_deferredSpot->bind();
                m_deferredRendering->bindGBufferTextures();
                m_spotShadowMap->bindTexture(SHADOW_MAP);

                m_deferredSpot->setUniform(S_SPOT_LIGHT ".point.base.color",      spotLight.color);
                m_deferredSpot->setUniform(S_SPOT_LIGHT ".point.base.intensity",  spotLight.intensity);
                m_deferredSpot->setUniform(S_SPOT_LIGHT ".point.atten.constant",  spotLight.getAttenuation().constant);
                m_deferredSpot->setUniform(S_SPOT_LIGHT ".point.atten.linear",    spotLight.getAttenuation().linear);
                m_deferredSpot->setUniform(S_SPOT_LIGHT ".point.atten.quadratic", spotLight.getAttenuation().quadratic);
                m_deferredSpot->setUniform(S_SPOT_LIGHT ".point.position",        transform.getPosition());
                m_deferredSpot->setUniform(S_SPOT_LIGHT ".point.range",           spotLight.getRange());
                m_deferredSpot->setUniform(S_SPOT_LIGHT ".direction",             transform.getDirection());
                m_deferredSpot->setUniform(S_SPOT_LIGHT ".cutoff",                glm::cos(spotLight.getCutOffAngle()));
                m_deferredSpot->setUniform("s_light_matrix",                      lightMatrix);

                m_deferredSpot->setUniform("g_mvp", mvp);
                m_lightBoundingCone.render(*m_deferredPoint);

                glCullFace(GL_BACK);
                glDisable(GL_BLEND);
            }
        }
        glDisable(GL_STENCIL_TEST);
    }

    void RenderingSystem::sortAlpha()
    {
        MG_PROFILE_ZONE_SCOPED;

        std::sort(m_alphaQueue.begin(), m_alphaQueue.end(), 
                  [this](Entity & obj1, Entity & obj2)
                  {
                     const auto pos1 = obj1.getComponent<TransformComponent>().getPosition();
                     const auto pos2 = obj2.getComponent<TransformComponent>().getPosition();

                     return glm::length(m_cameraPosition - pos1) >= glm::length(m_cameraPosition - pos2);
                  });
    }

    void RenderingSystem::addEntityToRenderQueue(Entity entity, ModelRendererComponent::RenderQueue renderQueue)
    {
        switch (renderQueue)
        {
            case ModelRendererComponent::RenderQueue::RQ_OPAQUE:
                m_opaqueQueue.push_back(entity);
                break;
            case ModelRendererComponent::RenderQueue::RQ_ALPHA:
                m_alphaQueue.push_back(entity);
                break;
            case ModelRendererComponent::RenderQueue::RQ_ENVIRO_MAPPING_STATIC:
                m_enviroStaticQueue.push_back(entity);
                break;
            case ModelRendererComponent::RenderQueue::RQ_ENVIRO_MAPPING_DYNAMIC:
                m_enviroDynamicQueue.push_back(entity);
                break;
        }
    }

    void RenderingSystem::removeEntityFromRenderQueue(Entity entity, ModelRendererComponent::RenderQueue renderQueue)
    {
        std::vector<Entity>::iterator entityIterator;

        switch (renderQueue)
        {
            case ModelRendererComponent::RenderQueue::RQ_OPAQUE:
                entityIterator = std::find(m_opaqueQueue.begin(), m_opaqueQueue.end(), entity);
                if (entityIterator != m_opaqueQueue.end())
                {
                    m_opaqueQueue.erase(entityIterator);
                }
                break;
            case ModelRendererComponent::RenderQueue::RQ_ALPHA:
                entityIterator = std::find(m_alphaQueue.begin(), m_alphaQueue.end(), entity);
                if (entityIterator != m_alphaQueue.end())
                {
                    m_alphaQueue.erase(entityIterator);
                }
                break;
            case ModelRendererComponent::RenderQueue::RQ_ENVIRO_MAPPING_STATIC:
                entityIterator = std::find(m_enviroStaticQueue.begin(), m_enviroStaticQueue.end(), entity);
                if (entityIterator != m_enviroStaticQueue.end())
                {
                    m_enviroStaticQueue.erase(entityIterator);
                }
                break;
            case ModelRendererComponent::RenderQueue::RQ_ENVIRO_MAPPING_DYNAMIC:
                entityIterator = std::find(m_enviroDynamicQueue.begin(), m_enviroDynamicQueue.end(), entity);
                if (entityIterator != m_enviroDynamicQueue.end())
                {
                    m_enviroDynamicQueue.erase(entityIterator);
                }
                break;
        }
    }

}
