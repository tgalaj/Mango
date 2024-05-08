#include "mgpch.h"

#include "RenderingSystem.h"
#include "Mango/Core/AssetManager.h"
#include "Mango/Rendering/BloomPS.h"
#include "Mango/Rendering/Debug/DebugMarkersGL.h"
#include "Mango/Rendering/Debug/DebugMesh.h"
#include "Mango/Rendering/DeferredRendering.h"
#include "Mango/Rendering/Picking.h"
#include "Mango/Rendering/SSAO.h"
#include "Mango/Rendering/ShaderGlobals.h"
#include "Mango/Rendering/JFAOutline.h"
#include "Mango/Scene/Components.h"
#include "Mango/Scene/SelectionManager.h"
#include "Mango/Window/Window.h"

namespace mango
{
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
        //Services::eventBus()->subscribe<ComponentAddedEvent<StaticMeshComponent>>(MG_BIND_EVENT(RenderingSystem::receive));
        //Services::eventBus()->subscribe<ComponentReplacedEvent<StaticMeshComponent>>(MG_BIND_EVENT(RenderingSystem::receive));
        //Services::eventBus()->subscribe<ComponentRemovedEvent<StaticMeshComponent>>(MG_BIND_EVENT(RenderingSystem::receive));
        Services::eventBus()->subscribe<ActiveSceneChangedEvent>(MG_BIND_EVENT(RenderingSystem::receive));

        // Init debug views
        addDebugTexture("None", nullptr);
        m_currentDebugView = getDebugView("None");

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

        m_wireframeShader = AssetManager::createShader("Wireframe", "Wireframe.vert", "Wireframe.frag");
        m_wireframeShader->link();

        m_billboardSpriteEditorShader = AssetManager::createShader("BillboardSprite", "BillboardSprite.vert", "BillboardSprite.frag", "BillboardSprite.geom");
        m_billboardSpriteEditorShader->link();

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

        m_deferredPoint = AssetManager::createShader("Deferred-Point", "DebugMesh.vert", "Deferred-Point.frag");
        m_deferredPoint->link();

        m_deferredSpot = AssetManager::createShader("Deferred-Spot", "DebugMesh.vert", "Deferred-Spot.frag");
        m_deferredSpot->link();

        m_debugMeshShader = AssetManager::createShader("DebugMesh", "DebugMesh.vert", "DebugMesh.frag");
        m_debugMeshShader->link();

        m_nullShader = AssetManager::createShader("NullShader", "DebugMesh.vert", "Shadow-Map-Gen.frag");
        m_nullShader->link();

        m_lightBoundingSphere = createRef<Mesh>();
        m_lightBoundingSphere->genSphere(1.1f, 36);

        m_lightBoundingCone      = DebugMesh::createDebugSpotLight();
        m_physicsColliderBox     = DebugMesh::createDebugBox();
        m_physicsColliderSphere  = DebugMesh::createDebugSphere();
        m_physicsColliderCapsule = DebugMesh::createDebugCapsule();
        m_debugSpotLightMesh     = DebugMesh::createDebugCone();
        m_debugDirLightMesh      = DebugMesh::createDebugDirLight();
        m_debugCameraFrustumMesh = DebugMesh::createDebugCameraFrustum();

        int width  = m_mainWindow->getWidth();
        int height = m_mainWindow->getHeight();

        s_DebugWindowWidth = GLuint(width / 5.0f);
        sceneAmbientColor  = glm::vec3(0.18f);

        m_hdrFilter = createRef<PostprocessEffect>();
        m_hdrFilter->init("HDR_PS", "HDR_PS.frag");

        m_fxaaFilter = createRef<PostprocessEffect>();
        m_fxaaFilter->init("FXAA_PS", "FXAA_PS.frag");

        m_deferredRendering = createRef<DeferredRendering>();
        m_deferredRendering->init();
        m_deferredRendering->createGBuffer(width, height);

        m_gbufferShader = AssetManager::getShader("GBuffer");

        m_bloomFilter = createRef<BloomPS>();
        m_bloomFilter->init("Bloom_PS", "Bloom_PS.frag");
        m_bloomFilter->create(width, height);

        m_ssao = createRef<SSAO>();
        m_ssao->init("SSAO_PS", "SSAO.frag");
        m_ssao->create(width, height);

        m_picking = createRef<Picking>();
        m_picking->init(width, height);

        m_jfaOutline = createRef<JFAOutline>();
        m_jfaOutline->init(width, height);

        m_mainRenderTarget = createRef<RenderTarget>();
        m_mainRenderTarget->create(width, height, RenderTarget::ColorInternalFormat::RGBA16F, RenderTarget::DepthInternalFormat::DEPTH32F_STENCIL8);

        m_helperRenderTarget = createRef<RenderTarget>();
        m_helperRenderTarget->create(width, height, RenderTarget::ColorInternalFormat::RGBA16F, RenderTarget::DepthInternalFormat::DEPTH32F_STENCIL8);

        m_dirShadowMap = createRef<RenderTarget>();
        m_dirShadowMap->create(2048, 2048, RenderTarget::DepthInternalFormat::DEPTH24);

        m_spotShadowMap = createRef<RenderTarget>();
        m_spotShadowMap->create(1024, 1024, RenderTarget::DepthInternalFormat::DEPTH24);

        m_omniShadowMap = createRef<RenderTarget>();
        m_omniShadowMap->create(512, 512, RenderTarget::DepthInternalFormat::DEPTH24, RenderTarget::RenderTargetType::TexCube);

        m_dirLightSpriteTexture = createRef<Texture>();
        m_dirLightSpriteTexture->createTexture2d("textures/DirLightSprite.png", false, 8);
        
        m_pointLightSpriteTexture = createRef<Texture>();
        m_pointLightSpriteTexture->createTexture2d("textures/PointLightSprite.png", false, 8);

        m_spotLightSpriteTexture = createRef<Texture>();
        m_spotLightSpriteTexture->createTexture2d("textures/SpotLightSprite.png", false, 8);

        m_cameraSpriteTexture = createRef<Texture>();
        m_cameraSpriteTexture->createTexture2d("textures/CameraSprite.png", false, 8);

        initRenderingStates();
    }

    void RenderingSystem::onUpdate(float dt)
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("RenderingSystem::onUpdate");

        if (!m_activeScene)
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
            return;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, m_outputToOffscreenTexture ? m_mainRenderTarget->m_fbo : 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        m_opaqueQueue.clear();
        m_alphaQueue.clear();
        m_enviroStaticQueue.clear();
        m_enviroDynamicQueue.clear();

        auto view = m_activeScene->getEntitiesWithComponent<StaticMeshComponent>();

        for (auto& e : view)
        {
             Entity entity = { e, m_activeScene };

            auto& smc = entity.getComponent<StaticMeshComponent>();

            if (!smc.mesh) continue;

            auto materialIndex = smc.mesh->getSubmesh()->materialIndex;
            auto renderQueue   = smc.materials[materialIndex]->getRenderQueue();
            
            addEntityToRenderQueue(entity, renderQueue);
        }

        // TODO: create two methods: renderGame and renderEditor + use switch
        // Or SceneRenderer class
        if (renderingMode == RenderingMode::GAME)
        {
            auto primaryCameraEntity = m_activeScene->getPrimaryCamera();

            if (!primaryCameraEntity) return;

            auto& cc = primaryCameraEntity.getComponent<CameraComponent>();
            auto& tc = primaryCameraEntity.getComponent<TransformComponent>();
            
            m_camera         = &cc.camera;
            m_cameraPosition = tc.getLocalPosition();

            // Update the view matrix of the primary camera
            glm::mat4 cameraRotation    = glm::mat4_cast(tc.getLocalOrientation());
            glm::mat4 cameraTranslation = glm::translate(glm::mat4(1.0f), -tc.getLocalPosition());

            m_camera->setView(cameraRotation * cameraTranslation);

            renderDeferred(m_activeScene);
        }

        if (renderingMode == RenderingMode::EDITOR)
        {
            renderDeferred(m_activeScene);

            // Draw the outline of the selected entity
            auto entity = SelectionManager::getSelectedEntity();
            if (entity)
            {
                m_jfaOutline->render(m_mainRenderTarget, entity, outlineColor, outlineWidth);
            }

            renderDebugView();
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

        if (event.entity.get().hasComponent<StaticMeshComponent>())
        {
            //auto& smc          = event.entity.get().getComponent<StaticMeshComponent>();
            //auto materialIndex = smc.mesh->getSubmesh().materialIndex;
            //auto renderQueue   = smc.materials[materialIndex]->getRenderQueue();
            //
            //removeEntityFromRenderQueue(event.entity, renderQueue);
        }
    }

    //void RenderingSystem::receive(const ComponentAddedEvent<StaticMeshComponent>& event)
    //{
    //    MG_PROFILE_ZONE_SCOPED;
    //    auto& smc          = event.entity.get().getComponent<StaticMeshComponent>();
    //    auto materialIndex = smc.mesh->getSubmesh().materialIndex;
    //    auto renderQueue   = smc.materials[materialIndex]->getRenderQueue();

    //    addEntityToRenderQueue(event.entity, renderQueue);
    //}

    //void RenderingSystem::receive(const ComponentReplacedEvent<StaticMeshComponent>& event)
    //{
    //    MG_PROFILE_ZONE_SCOPED;
    //    auto& smc          = event.entity.get().getComponent<StaticMeshComponent>();
    //    auto materialIndex = smc.mesh->getSubmesh().materialIndex;
    //    auto renderQueue   = smc.materials[materialIndex]->getRenderQueue();

    //    removeEntityFromRenderQueue(event.entity, renderQueue);
    //    addEntityToRenderQueue     (event.entity, renderQueue);
    //}

    //void RenderingSystem::receive(const ComponentRemovedEvent<StaticMeshComponent>& event)
    //{
    //    MG_PROFILE_ZONE_SCOPED;
    //    auto& smc          = event.entity.get().getComponent<StaticMeshComponent>();
    //    auto materialIndex = smc.mesh->getSubmesh().materialIndex;
    //    auto renderQueue   = smc.materials[materialIndex]->getRenderQueue();

    //    removeEntityFromRenderQueue(event.entity, renderQueue);
    //}

    void RenderingSystem::receive(const ActiveSceneChangedEvent& event)
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_CORE_ASSERT_MSG(event.scene != nullptr, "New active scene pointer is nullptr.");

        m_opaqueQueue.clear();
        m_alphaQueue.clear();
        m_enviroStaticQueue.clear();
        m_enviroDynamicQueue.clear();

        m_activeScene = event.scene;

        auto view = m_activeScene->getEntitiesWithComponent<StaticMeshComponent>();
        for (auto e : view)
        {
            //Entity entity        = { e, m_activeScene };
            //auto&  smc           = entity.getComponent<StaticMeshComponent>();
            //auto   materialIndex = smc.mesh->getSubmesh().materialIndex;
            //auto   renderQueue   = smc.materials[materialIndex]->getRenderQueue();

            //addEntityToRenderQueue(entity, renderQueue);
        }
    }

    void RenderingSystem::setSkybox(const ref<Skybox>& skybox)
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
        m_jfaOutline->resize(width, height);

        s_DebugWindowWidth = GLuint(width / 5.0f);

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

        // Regular objects
        auto pickingShader = m_picking->getShader();
        pickingShader->bind();

        auto view = m_activeScene->getEntitiesWithComponent<TransformComponent, StaticMeshComponent>();
        for (auto entity : view)
        {
            auto [tc, smc] = view.get<TransformComponent, StaticMeshComponent>(entity);

            pickingShader->updateGlobalUniforms(tc);
            int id = (int)entity;
            pickingShader->setUniform("objectID", (int)id);
            smc.mesh->bind();
            
            auto& submeshes = smc.mesh->getSubmeshes();
            for (uint32_t submeshIndex = 0; submeshIndex < submeshes.size(); ++submeshIndex)
            {
                smc.mesh->render(submeshIndex);
            }
        }
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        // Light and camera billboards
        auto pickingBillboardShader = m_picking->getBillboardShader();
        pickingBillboardShader->bind();
        pickingBillboardShader->setUniform("half_quad_width_vs", 0.5f);

        {
            auto view = m_activeScene->getEntitiesWithComponent<TransformComponent, DirectionalLightComponent>();
            for (auto entity : view)
            {
                auto& tc = view.get<TransformComponent>(entity);

                pickingBillboardShader->updateGlobalUniforms(tc);
                int id = (int)entity;
                pickingBillboardShader->setUniform("position", tc.getPosition());
                pickingBillboardShader->setUniform("objectID", (int)id);

                glDrawArrays(GL_POINTS, 0, 1);
            }
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        }

        {
            auto view = m_activeScene->getEntitiesWithComponent<TransformComponent, PointLightComponent>();
            for (auto entity : view)
            {
                auto& tc = view.get<TransformComponent>(entity);

                pickingBillboardShader->updateGlobalUniforms(tc);
                int id = (int)entity;
                pickingBillboardShader->setUniform("position", tc.getPosition());
                pickingBillboardShader->setUniform("objectID", (int)id);

                glDrawArrays(GL_POINTS, 0, 1);
            }
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        }

        {
            auto view = m_activeScene->getEntitiesWithComponent<TransformComponent, SpotLightComponent>();
            for (auto entity : view)
            {
                auto& tc = view.get<TransformComponent>(entity);

                pickingBillboardShader->updateGlobalUniforms(tc);
                int id = (int)entity;
                pickingBillboardShader->setUniform("position", tc.getPosition());
                pickingBillboardShader->setUniform("objectID", (int)id);

                glDrawArrays(GL_POINTS, 0, 1);
            }
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        }

        {
            auto view = m_activeScene->getEntitiesWithComponent<TransformComponent, CameraComponent>();
            for (auto entity : view)
            {
                auto& tc = view.get<TransformComponent>(entity);

                pickingBillboardShader->updateGlobalUniforms(tc);
                int id = (int)entity;
                pickingBillboardShader->setUniform("position", tc.getPosition());
                pickingBillboardShader->setUniform("objectID", (int)id);

                glDrawArrays(GL_POINTS, 0, 1);
            }
            glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
        }

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_SCISSOR_TEST);

        m_picking->setPickingRegion(0, 0, m_mainRenderTarget->getWidth(), m_mainRenderTarget->getHeight());

        return m_picking->getPickedID();
    }

    uint32_t RenderingSystem::getOutputOffscreenTextureID() const
    {
        return m_mainRenderTarget->m_textures[0]->getRendererID();
    }

    void RenderingSystem::setRenderingMode(RenderingMode mode, Camera* editorCamera /*= nullptr*/, const glm::vec3& editorCameraPosition /*= glm::vec3(0.0f)*/)
    {
        renderingMode           = mode;
        m_camera         = editorCamera;
        m_cameraPosition = editorCameraPosition;
    }

    Camera& RenderingSystem::getCamera() const
    {
        return *(m_camera);
    }

    void RenderingSystem::addDebugTexture(const std::string& viewName, const ref<Texture>& texture)
    {
        m_debugViews[viewName] = texture;
    }

    void RenderingSystem::setCurrentDebugView(const std::string& viewName)
    {
            m_currentDebugView = getDebugView(viewName);
    }

    DebugView RenderingSystem::getDebugView(const std::string& viewName) const
    {
        if (m_debugViews.contains(viewName))
        {
            return { viewName, m_debugViews.at(viewName)};
        }

        return { "None", nullptr };
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

    void RenderingSystem::applyPostprocess(ref<PostprocessEffect> & effect, 
                                           ref<RenderTarget>      * src, 
                                           ref<RenderTarget>      * dst)
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
        renderEntitiesInQueue(m_forwardAmbient, m_opaqueQueue);

        renderLightsForward(scene);

        /* Sort transparent objects back to front */
        sortAlpha();

        /* Render transparent objects */
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_CULL_FACE);
        m_blendingShader->bind();
        renderEntitiesInQueue(m_blendingShader, m_alphaQueue);
        glEnable(GL_CULL_FACE);

        if (s_VisualizeLights)
        {
            renderDebugLightsBoundingBoxes(scene);
        }

        if (s_VisualizePhysicsColliders)
        {
            renderDebugPhysicsColliders(scene);
        }

        /* Render skybox */
        if (m_skybox != nullptr)
        {
            m_skybox->render(getCamera().getProjection(), getCamera().getView());

            m_enviroMappingShader->bind();
            m_enviroMappingShader->setSubroutine(Shader::Type::FRAGMENT, "reflection"); // TODO: control this using Material class

            m_skybox->bindSkyboxTexture();
            renderEntitiesInQueue(m_enviroMappingShader, m_enviroStaticQueue);
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

        glClearColor(0.0, 0.0, 0.0, 1.0);

        m_deferredRendering->bindGBuffer();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (ShadingMode::SHADED == s_ShadingMode || ShadingMode::SHADED_WIREFRAME == s_ShadingMode)
        {
            m_gbufferShader->bind();
            renderEntitiesInQueue(m_gbufferShader, m_opaqueQueue);

            /* Compute SSAO */
            m_ssao->computeSSAO(m_deferredRendering, getCamera().getView(), getCamera().getProjection());
            m_ssao->blurSSAO();

            glDepthMask(GL_FALSE);

            /* Light Pass - compute lighting */
            m_mainRenderTarget->bind();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            m_deferredRendering->bindGBufferReadOnly();
            m_mainRenderTarget->bindWriteOnly();
            glBlitFramebuffer(0, 0, m_mainWindow->getWidth(), m_mainWindow->getHeight(),
                              0, 0, m_mainWindow->getWidth(), m_mainWindow->getHeight(),
                              GL_DEPTH_BUFFER_BIT, GL_NEAREST);

            m_ssao->bindBlurredSSAOTexture(9); //TODO: replace magic number with a variable
            renderLightsDeferred(scene);

            m_mainRenderTarget->bind();
            glEnable(GL_DEPTH_TEST);
            glDepthMask(GL_TRUE);

            if (renderingMode == RenderingMode::EDITOR)
            {
                if (s_VisualizeLights)
                {
                    renderDebugLightsBoundingBoxes(scene);
                }

                if (s_VisualizePhysicsColliders)
                {
                    renderDebugPhysicsColliders(scene);
                }

                renderLightBillboards(scene);
            }

            /* Sort transparent objects back to front */
            sortAlpha();

            /* Render transparent objects */
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDisable(GL_CULL_FACE);
            m_blendingShader->bind();
            renderEntitiesInQueue(m_blendingShader, m_alphaQueue);
            glEnable(GL_CULL_FACE);

            /* Render skybox */
            if (m_skybox != nullptr)
            {
                m_skybox->render(getCamera().getProjection(), getCamera().getView());

                m_enviroMappingShader->bind();
                //m_enviro_mapping_shader->setSubroutine(Shader::Type::FRAGMENT, "refraction"); // TODO: control this using Material class
                m_enviroMappingShader->setSubroutine(Shader::Type::FRAGMENT, "reflection");

                m_skybox->bindSkyboxTexture();
                renderEntitiesInQueue(m_enviroMappingShader, m_enviroStaticQueue);
            }

            /* Apply postprocess effect */
            m_bloomFilter->extractBrightness(m_mainRenderTarget, 1.0);
            m_bloomFilter->blurGaussian(2);
        }
        
        if (ShadingMode::WIREFRAME == s_ShadingMode || ShadingMode::SHADED_WIREFRAME == s_ShadingMode)
        {
            if (ShadingMode::WIREFRAME == s_ShadingMode)
            {
                m_bloomFilter->clearBuffers();
            }

            glClearColor(0.05, 0.05, 0.05, 1.0);

            m_mainRenderTarget->bind();
            m_wireframeShader->bind();
            
            glEnable(GL_POLYGON_OFFSET_LINE);
            glPolygonOffset(-1.5, -1.0);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

            renderEntitiesInQueue(m_wireframeShader, m_opaqueQueue);
            renderEntitiesInQueue(m_wireframeShader, m_alphaQueue);
            renderEntitiesInQueue(m_wireframeShader, m_enviroStaticQueue);
            renderEntitiesInQueue(m_wireframeShader, m_enviroDynamicQueue);

            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            glDisable(GL_POLYGON_OFFSET_LINE);
        }

        m_bloomFilter->bindBrightnessTexture(1);
        applyPostprocess(m_hdrFilter, &m_mainRenderTarget, &m_helperRenderTarget);
        applyPostprocess(m_fxaaFilter, &m_helperRenderTarget, m_outputToOffscreenTexture ? &m_mainRenderTarget : 0);
    }

    void RenderingSystem::renderDebugView()
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("RenderingSystem::renderDebug");

        auto debugViewTexture = m_currentDebugView.second;
        if (!debugViewTexture) return; // early exit

        glDisable(GL_BLEND);
        glClear(GL_DEPTH_BUFFER_BIT);

        m_debugRendering->bind();

        // Note: editor camera is always set to perspective
        m_debugRendering->setUniform("nearZ", getCamera().getPerspectiveNearClip());
        m_debugRendering->setUniform("farZ",  getCamera().getPerspectiveFarClip());

        auto format = debugViewTexture->getDescriptor().format;
        bool isDepth  = (RenderTarget::DepthInternalFormat)format == RenderTarget::DepthInternalFormat::DEPTH16;
             isDepth |= (RenderTarget::DepthInternalFormat)format == RenderTarget::DepthInternalFormat::DEPTH24;
             isDepth |= (RenderTarget::DepthInternalFormat)format == RenderTarget::DepthInternalFormat::DEPTH32;
             isDepth |= (RenderTarget::DepthInternalFormat)format == RenderTarget::DepthInternalFormat::DEPTH32F;
             isDepth |= (RenderTarget::DepthInternalFormat)format == RenderTarget::DepthInternalFormat::DEPTH24_STENCIL8;
             isDepth |= (RenderTarget::DepthInternalFormat)format == RenderTarget::DepthInternalFormat::DEPTH32F_STENCIL8;
             isDepth |= (RenderTarget::DepthInternalFormat)format == RenderTarget::DepthInternalFormat::STENCIL_INDEX8;

        if (isDepth)
        {
            m_debugRendering->setSubroutine(Shader::Type::FRAGMENT, "debugDepthTarget");
        }
        else
        {
            m_debugRendering->setSubroutine(Shader::Type::FRAGMENT, "debugColorTarget");
        }
 
        debugViewTexture->bind(0);
        glViewport(0, 0, m_mainFramebufferSize.x, m_mainFramebufferSize.y);
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
            auto view = scene->getEntitiesWithComponent<PointLightComponent, TransformComponent>();
            for (auto entity : view)
            {
                auto [pointLight, transform] = view.get(entity);

                auto model        = glm::translate(glm::mat4(1.0f), transform.getPosition()) *
                                    glm::scale(glm::mat4(1.0f), glm::vec3(pointLight.getRange()));
                auto& view        = getCamera().getView();
                auto& projection  = getCamera().getProjection();

                m_debugMeshShader->bind();
                m_debugMeshShader->setUniform("g_mvp", projection * view * model);
                m_debugMeshShader->setUniform("color", pointLight.color);

                m_physicsColliderSphere->bind();
                m_physicsColliderSphere->render();
            }
        }

        /* Spot Lights */
        {
            auto view = scene->getEntitiesWithComponent<SpotLightComponent, TransformComponent>();
            for (auto entity : view)
            {
                auto [spotLight, transform] = view.get(entity);

                float heightScale = spotLight.getRange();
                float radiusScale = spotLight.getRange() * glm::tan(spotLight.getCutOffAngle()); 

                auto model      = glm::translate(glm::mat4(1.0f), transform.getPosition()) *
                                  glm::mat4_cast(transform.getOrientation()) *
                                  glm::scale(glm::mat4(1.0f), glm::vec3(radiusScale, radiusScale, heightScale));
                auto view       = getCamera().getView();
                auto projection = getCamera().getProjection();

                m_debugMeshShader->bind();
                m_debugMeshShader->setUniform("g_mvp", projection * view * model);
                m_debugMeshShader->setUniform("color", spotLight.color);

                m_debugSpotLightMesh->bind();
                m_debugSpotLightMesh->render();
            }
        }

        /* Directional Lights */
        {
            auto view = scene->getEntitiesWithComponent<DirectionalLightComponent, TransformComponent>();
            for (auto entity : view)
            {
                auto [dirLight, transform] = view.get(entity);

                float scaleFactor = length(transform.getPosition() - m_cameraPosition) * m_camera->getPerspectiveVerticalFieldOfView() * 0.05f;

                auto model      = glm::translate(glm::mat4(1.0f), transform.getPosition()) *
                                  glm::mat4_cast(glm::inverse(transform.getOrientation())) *
                                  glm::scale(glm::mat4(1.0f), glm::vec3(scaleFactor));
                auto view       = getCamera().getView();
                auto projection = getCamera().getProjection();

                m_debugMeshShader->bind();
                m_debugMeshShader->setUniform("g_mvp", projection * view * model);
                m_debugMeshShader->setUniform("color", dirLight.color);

                m_debugDirLightMesh->bind();
                m_debugDirLightMesh->render();
            }
        }

        /* Camera Frustum */
        {
            auto view = scene->getEntitiesWithComponent<CameraComponent, TransformComponent>();
            m_debugMeshShader->setUniform("color", glm::vec3(1.0f, 1.0f, 1.0f));

            for (auto entity : view)
            {
                auto [camera, transform] = view.get(entity);

                auto model = glm::translate(glm::mat4(1.0f), transform.getPosition()) *
                             glm::mat4_cast(glm::inverse(transform.getOrientation())) * 
                             glm::inverse(camera.camera.getProjection());

                auto view       = getCamera().getView();
                auto projection = getCamera().getProjection();

                m_debugMeshShader->bind();
                m_debugMeshShader->setUniform("g_mvp", projection * view * model);

                m_debugCameraFrustumMesh->bind();
                m_debugCameraFrustumMesh->render();
            }
        }

        glEnable(GL_BLEND);
    }

    void RenderingSystem::renderDebugPhysicsColliders(Scene* scene)
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("RenderingSystem::renderDebugPhysicsColliders");

        glDisable(GL_BLEND);
        
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(-1.0, -1.0);

        Entity selectedEntity = SelectionManager::getSelectedEntity();

        if (selectedEntity)
        {
            if (selectedEntity.hasComponent<BoxCollider3DComponent>()) 
            {
                auto& bc3d = selectedEntity.getComponent<BoxCollider3DComponent>();

                auto model       = glm::translate(glm::mat4(1.0f), selectedEntity.getPosition() + bc3d.offset) *
                                   glm::mat4_cast(selectedEntity.getOrientation()) * 
                                   glm::scale(glm::mat4(1.0f), selectedEntity.getScale() * bc3d.halfExtent);
                auto& view       = getCamera().getView();
                auto& projection = getCamera().getProjection();

                m_debugMeshShader->bind();
                m_debugMeshShader->setUniform("g_mvp", projection * view * model);
                m_debugMeshShader->setUniform("color", s_PhysicsCollidersColor);

                m_physicsColliderBox->bind();
                m_physicsColliderBox->render();
            }
            else if (selectedEntity.hasComponent<CapsuleColliderComponent>())
            {
                auto& cc = selectedEntity.getComponent<CapsuleColliderComponent>();

                auto scale             = selectedEntity.getScale();
                auto maxScaleComponent = glm::vec3(glm::max(scale.x, scale.z));
                auto model             = glm::translate(glm::mat4(1.0f), selectedEntity.getPosition() + cc.offset) *
                                         glm::mat4_cast(selectedEntity.getOrientation()) *
                                         glm::scale(glm::mat4(1.0f), maxScaleComponent * 2.0f * glm::vec3(cc.radius, cc.halfHeight, cc.radius));
                auto& view             = getCamera().getView();
                auto& projection       = getCamera().getProjection();

                m_debugMeshShader->bind();
                m_debugMeshShader->setUniform("g_mvp", projection * view * model);
                m_debugMeshShader->setUniform("color", s_PhysicsCollidersColor);

                m_physicsColliderCapsule->bind();
                m_physicsColliderCapsule->render();
            }
            else if (selectedEntity.hasComponent<SphereColliderComponent>())
            {
                auto& sc = selectedEntity.getComponent<SphereColliderComponent>();

                auto scale             = selectedEntity.getScale();
                auto maxScaleComponent = glm::vec3(glm::max(scale.x, glm::max(scale.y, scale.z)));
                auto model             = glm::translate(glm::mat4(1.0f), selectedEntity.getPosition() + sc.offset) *
                                         glm::mat4_cast(selectedEntity.getOrientation()) *
                                         glm::scale(glm::mat4(1.0f), maxScaleComponent * sc.radius);
                auto& view             = getCamera().getView();
                auto& projection       = getCamera().getProjection();

                m_debugMeshShader->bind();
                m_debugMeshShader->setUniform("g_mvp", projection * view * model);
                m_debugMeshShader->setUniform("color", s_PhysicsCollidersColor);

                m_physicsColliderSphere->bind();
                m_physicsColliderSphere->render();
            }
        }

        glDisable(GL_POLYGON_OFFSET_FILL);
        glEnable(GL_BLEND);
    }

    void RenderingSystem::renderLightBillboards(Scene* scene)
    {
        MG_BEGIN_GL_MARKER("Draw light billboards");

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        m_billboardSpriteEditorShader->bind();
        m_billboardSpriteEditorShader->setUniform("half_quad_width_vs", 0.5f);

        // Directional Lights
        {
            auto view = m_activeScene->getEntitiesWithComponent<DirectionalLightComponent, TransformComponent>();

            m_dirLightSpriteTexture->bind(0);
            for (auto& e : view)
            {
                auto [light, transform] = view.get(e);
                m_billboardSpriteEditorShader->updateGlobalUniforms(transform);
                m_billboardSpriteEditorShader->setUniform("position", transform.getPosition());
                m_billboardSpriteEditorShader->setUniform("color", light.color);
                glDrawArrays(GL_POINTS, 0, 1);
            }
        }

        // Point Lights
        {
            auto view = m_activeScene->getEntitiesWithComponent<PointLightComponent, TransformComponent>();

            m_pointLightSpriteTexture->bind(0);
            for (auto& e : view)
            {
                auto [light, transform] = view.get(e);
                m_billboardSpriteEditorShader->updateGlobalUniforms(transform);
                m_billboardSpriteEditorShader->setUniform("position", transform.getPosition());
                m_billboardSpriteEditorShader->setUniform("color", light.color);
                glDrawArrays(GL_POINTS, 0, 1);
            }
        }

        // Spot Lights
        {
            auto view = m_activeScene->getEntitiesWithComponent<SpotLightComponent, TransformComponent>();

            m_spotLightSpriteTexture->bind(0);
            for (auto& e : view)
            {
                auto [light, transform] = view.get(e);
                m_billboardSpriteEditorShader->updateGlobalUniforms(transform);
                m_billboardSpriteEditorShader->setUniform("position", transform.getPosition());
                m_billboardSpriteEditorShader->setUniform("color", light.color);
                glDrawArrays(GL_POINTS, 0, 1);
            }
        }

        // Cameras
        {
            auto view = m_activeScene->getEntitiesWithComponent<CameraComponent, TransformComponent>();

            m_cameraSpriteTexture->bind(0);
            m_billboardSpriteEditorShader->setUniform("color", glm::vec3(1.0f));

            for (auto& e : view)
            {
                auto& transform = view.get<TransformComponent>(e);
                m_billboardSpriteEditorShader->updateGlobalUniforms(transform);
                m_billboardSpriteEditorShader->setUniform("position", transform.getPosition());
                glDrawArrays(GL_POINTS, 0, 1);
            }
        }

        glDisable(GL_BLEND);
        MG_END_GL_MARKER;
    }

    void RenderingSystem::renderEntitiesInQueue(ref<Shader>& shader, std::vector<Entity>& queue)
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("RenderingSystem::renderOpaque");

        for (auto& entity : queue)
        {
            auto& smc  = entity.getComponent<StaticMeshComponent>();
            auto& tc   = entity.getComponent<TransformComponent>();
            auto& mesh = smc.mesh;

            mesh->bind();
            shader->bind();
            shader->updateGlobalUniforms(tc);

            auto& submeshes = mesh->getSubmeshes();
            for (uint32_t submeshIndex = 0; submeshIndex < submeshes.size(); ++submeshIndex)
            {
                auto materialIndex = submeshes[submeshIndex].materialIndex;
                MG_CORE_ASSERT(materialIndex < smc.materials.size());

                auto& material = smc.materials[materialIndex];
                if (material)
                {
                    for (auto const& [texture_type, texture] : material->getTextureMap())
                    {
                        texture->bind(uint32_t(texture_type));
                    }

                    // Set uniforms based on the data in the material
                    for (auto& [uniform_name, value] : material->getBoolMap())
                    {
                        shader->setUniform(uniform_name, value);
                    }

                    for (auto& [uniform_name, value] : material->getFloatMap())
                    {
                        shader->setUniform(uniform_name, value);
                    }

                    for (auto& [uniform_name, value] : material->getVec3Map())
                    {
                        shader->setUniform(uniform_name, value);
                    }
                }
                mesh->render(submeshIndex);
            }
        }
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

                    lightMatrix = shadowInfo.getProjection() * glm::lookAt(-transform.getForward(), glm::vec3(0.0f), glm::vec3(0, 1, 0));
                    m_shadowMapGenerator->setUniform("s_light_matrix", lightMatrix);

                    glCullFace(GL_FRONT);
                    renderEntitiesInQueue(m_shadowMapGenerator, m_opaqueQueue);
                    renderEntitiesInQueue(m_shadowMapGenerator, m_enviroStaticQueue);
                    glCullFace(GL_BACK);
                }

                bindMainRenderTarget();
            
                m_forwardDirectional->bind();
                m_dirShadowMap->bindTexture(SHADOW_MAP);

                m_forwardDirectional->setUniform(S_DIRECTIONAL_LIGHT ".base.color",     directionalLight.color);
                m_forwardDirectional->setUniform(S_DIRECTIONAL_LIGHT ".base.intensity", directionalLight.intensity);
                m_forwardDirectional->setUniform(S_DIRECTIONAL_LIGHT ".direction",      transform.getForward());
                m_forwardDirectional->setUniform("s_light_matrix", lightMatrix);

                beginForwardRendering();
                renderEntitiesInQueue(m_forwardDirectional, m_opaqueQueue);
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

                    lightMatrices[0] = shadowInfo.getProjection() * glm::lookAt(transform.getLocalPosition(), transform.getLocalPosition() + glm::vec3( 1,  0,  0), glm::vec3(0, -1,  0));
                    lightMatrices[1] = shadowInfo.getProjection() * glm::lookAt(transform.getLocalPosition(), transform.getLocalPosition() + glm::vec3(-1,  0,  0), glm::vec3(0, -1,  0));
                    lightMatrices[2] = shadowInfo.getProjection() * glm::lookAt(transform.getLocalPosition(), transform.getLocalPosition() + glm::vec3( 0,  1,  0), glm::vec3(0,  0,  1));
                    lightMatrices[3] = shadowInfo.getProjection() * glm::lookAt(transform.getLocalPosition(), transform.getLocalPosition() + glm::vec3( 0, -1,  0), glm::vec3(0,  0, -1));
                    lightMatrices[4] = shadowInfo.getProjection() * glm::lookAt(transform.getLocalPosition(), transform.getLocalPosition() + glm::vec3( 0,  0,  1), glm::vec3(0, -1,  0));
                    lightMatrices[5] = shadowInfo.getProjection() * glm::lookAt(transform.getLocalPosition(), transform.getLocalPosition() + glm::vec3( 0,  0, -1), glm::vec3(0, -1,  0));

                    m_omniShadowMapGenerator->setUniform("s_light_matrices", lightMatrices, 6);
                    m_omniShadowMapGenerator->setUniform("s_light_pos",      transform.getLocalPosition());
                    m_omniShadowMapGenerator->setUniform("s_far_plane",      100.0f);

                    glCullFace(GL_FRONT);
                    renderEntitiesInQueue(m_omniShadowMapGenerator, m_opaqueQueue);
                    renderEntitiesInQueue(m_omniShadowMapGenerator, m_enviroStaticQueue);
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
                m_forwardPoint->setUniform(S_POINT_LIGHT ".position",        transform.getLocalPosition());
                m_forwardPoint->setUniform(S_POINT_LIGHT ".range",           pointLight.getRange());
                m_forwardPoint->setUniform("s_far_plane", 100.0f);

                beginForwardRendering();
                renderEntitiesInQueue(m_forwardPoint, m_opaqueQueue);
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

                    lightMatrix = shadowInfo.getProjection() * glm::lookAt(transform.getPosition(), transform.getPosition() + transform.getForward(), glm::vec3(0, 1, 0));
                    m_shadowMapGenerator->setUniform("s_light_matrix", lightMatrix);

                    glCullFace(GL_FRONT);
                    renderEntitiesInQueue(m_shadowMapGenerator, m_opaqueQueue);
                    renderEntitiesInQueue(m_shadowMapGenerator, m_enviroStaticQueue);
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
                m_forwardSpot->setUniform(S_SPOT_LIGHT ".direction",             transform.getForward());
                m_forwardSpot->setUniform(S_SPOT_LIGHT ".cutoff",                glm::cos(spotLight.getCutOffAngle()));
                m_forwardSpot->setUniform("s_light_matrix",                      lightMatrix);

                beginForwardRendering();
                renderEntitiesInQueue(m_forwardSpot, m_opaqueQueue);
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

                    lightMatrix = shadowInfo.getProjection() * glm::lookAt(-transform.getForward(), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                    m_shadowMapGenerator->setUniform("s_light_matrix", lightMatrix);

                    glCullFace(GL_FRONT);
                    renderEntitiesInQueue(m_shadowMapGenerator, m_opaqueQueue);
                    renderEntitiesInQueue(m_shadowMapGenerator, m_enviroStaticQueue);
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
                m_deferredDirectional->setUniform(S_DIRECTIONAL_LIGHT ".direction",      transform.getForward());
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
                    lightMatrices[0] = shadowInfo.getProjection() * glm::lookAt(transform.getLocalPosition(), transform.getLocalPosition() + glm::vec3( 1,  0,  0), glm::vec3(0, -1,  0));
                    lightMatrices[1] = shadowInfo.getProjection() * glm::lookAt(transform.getLocalPosition(), transform.getLocalPosition() + glm::vec3(-1,  0,  0), glm::vec3(0, -1,  0));
                    lightMatrices[2] = shadowInfo.getProjection() * glm::lookAt(transform.getLocalPosition(), transform.getLocalPosition() + glm::vec3( 0,  1,  0), glm::vec3(0,  0,  1));
                    lightMatrices[3] = shadowInfo.getProjection() * glm::lookAt(transform.getLocalPosition(), transform.getLocalPosition() + glm::vec3( 0, -1,  0), glm::vec3(0,  0, -1));
                    lightMatrices[4] = shadowInfo.getProjection() * glm::lookAt(transform.getLocalPosition(), transform.getLocalPosition() + glm::vec3( 0,  0,  1), glm::vec3(0, -1,  0));
                    lightMatrices[5] = shadowInfo.getProjection() * glm::lookAt(transform.getLocalPosition(), transform.getLocalPosition() + glm::vec3( 0,  0, -1), glm::vec3(0, -1,  0));

                    m_omniShadowMapGenerator->setUniform("s_light_matrices", lightMatrices, 6);
                    m_omniShadowMapGenerator->setUniform("s_light_pos", transform.getLocalPosition());
                    m_omniShadowMapGenerator->setUniform("s_far_plane", 100.0f);

                    glCullFace(GL_FRONT);
                    renderEntitiesInQueue(m_omniShadowMapGenerator, m_opaqueQueue);
                    renderEntitiesInQueue(m_omniShadowMapGenerator, m_enviroStaticQueue);
                    glCullFace(GL_BACK);

                    glDepthMask(GL_FALSE);
                    glDisable(GL_DEPTH_TEST);
                }

                bindMainRenderTarget();

                /* Bounding sphere MVP matrix setup */
                auto model = glm::translate(glm::mat4(1.0f), transform.getLocalPosition()) *
                             glm::scale    (glm::mat4(1.0f), glm::vec3(pointLight.getRange()));

                auto& view       = getCamera().getView();
                auto& projection = getCamera().getProjection();
                auto  mvp        = projection * view * model;

                /* Stencil pass */
                m_nullShader->bind();

                glEnable(GL_DEPTH_TEST);
                glDisable(GL_CULL_FACE);

                glClear(GL_STENCIL_BUFFER_BIT);
                glStencilFunc(GL_ALWAYS, 0, 0xFF);
                glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
                glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

                m_nullShader->setUniform("g_mvp", mvp);
                m_lightBoundingSphere->bind();
                m_lightBoundingSphere->render();

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
                m_deferredPoint->setUniform(S_POINT_LIGHT ".position",        transform.getLocalPosition());
                m_deferredPoint->setUniform(S_POINT_LIGHT ".range",           pointLight.getRange());
                m_deferredPoint->setUniform("s_far_plane",                    pointLight.getShadowFarPlane());

                m_deferredPoint->setUniform("g_mvp", mvp);
                m_lightBoundingSphere->bind();
                m_lightBoundingSphere->render();

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

                    lightMatrix = shadowInfo.getProjection() * glm::lookAt(transform.getPosition(), transform.getPosition() + transform.getForward(), glm::vec3(0, 1, 0));
                    m_shadowMapGenerator->setUniform("s_light_matrix", lightMatrix);

                    glCullFace(GL_FRONT);
                    renderEntitiesInQueue(m_shadowMapGenerator, m_opaqueQueue);
                    renderEntitiesInQueue(m_shadowMapGenerator, m_enviroStaticQueue);
                    glCullFace(GL_BACK);

                    glDepthMask(GL_FALSE);
                    glDisable(GL_DEPTH_TEST);
                }

                bindMainRenderTarget();

                /* Bounding cone MVP matrix setup */
                float heightScale = spotLight.getRange();
                float radiusScale = spotLight.getRange() * glm::tan(spotLight.getCutOffAngle());

                auto model = glm::translate(glm::mat4(1.0f), transform.getPosition()) *
                             glm::mat4_cast(transform.getOrientation()) *
                             glm::scale    (glm::mat4(1.0f), glm::vec3(radiusScale, radiusScale, heightScale));

                auto view       = getCamera().getView();
                auto projection = getCamera().getProjection();
                auto mvp        = projection * view * model;

                /* Stencil pass */
                m_nullShader->bind();

                glEnable(GL_DEPTH_TEST);
                glDisable(GL_CULL_FACE);

                glClear(GL_STENCIL_BUFFER_BIT);
                glStencilFunc(GL_ALWAYS, 0, 0);
                glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
                glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

                m_nullShader->setUniform("g_mvp", mvp);
                m_lightBoundingCone->bind();
                m_lightBoundingCone->render();

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
                m_deferredSpot->setUniform(S_SPOT_LIGHT ".point.position",        transform.getLocalPosition());
                m_deferredSpot->setUniform(S_SPOT_LIGHT ".point.range",           spotLight.getRange());
                m_deferredSpot->setUniform(S_SPOT_LIGHT ".direction",             transform.getForward());
                m_deferredSpot->setUniform(S_SPOT_LIGHT ".cutoff",                glm::cos(spotLight.getCutOffAngle()));
                m_deferredSpot->setUniform("s_light_matrix",                      lightMatrix);

                m_deferredSpot->setUniform("g_mvp", mvp);
                m_lightBoundingCone->bind();
                m_lightBoundingCone->render();

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
                     const auto pos1 = obj1.getComponent<TransformComponent>().getLocalPosition();
                     const auto pos2 = obj2.getComponent<TransformComponent>().getLocalPosition();

                     return glm::length(m_cameraPosition - pos1) >= glm::length(m_cameraPosition - pos2);
                  });
    }

    void RenderingSystem::addEntityToRenderQueue(Entity entity, Material::RenderQueue renderQueue)
    {
        switch (renderQueue)
        {
            case Material::RenderQueue::RQ_OPAQUE:
                m_opaqueQueue.push_back(entity);
                break;
            case Material::RenderQueue::RQ_TRANSPARENT:
                m_alphaQueue.push_back(entity);
                break;
            case Material::RenderQueue::RQ_ENVIRO_MAPPING_STATIC:
                m_enviroStaticQueue.push_back(entity);
                break;
            case Material::RenderQueue::RQ_ENVIRO_MAPPING_DYNAMIC:
                m_enviroDynamicQueue.push_back(entity);
                break;
        }
    }

    void RenderingSystem::removeEntityFromRenderQueue(Entity entity, Material::RenderQueue renderQueue)
    {
        std::vector<Entity>::iterator entityIterator;

        switch (renderQueue)
        {
            case Material::RenderQueue::RQ_OPAQUE:
                entityIterator = std::find(m_opaqueQueue.begin(), m_opaqueQueue.end(), entity);
                if (entityIterator != m_opaqueQueue.end())
                {
                    m_opaqueQueue.erase(entityIterator);
                }
                break;
            case Material::RenderQueue::RQ_TRANSPARENT:
                entityIterator = std::find(m_alphaQueue.begin(), m_alphaQueue.end(), entity);
                if (entityIterator != m_alphaQueue.end())
                {
                    m_alphaQueue.erase(entityIterator);
                }
                break;
            case Material::RenderQueue::RQ_ENVIRO_MAPPING_STATIC:
                entityIterator = std::find(m_enviroStaticQueue.begin(), m_enviroStaticQueue.end(), entity);
                if (entityIterator != m_enviroStaticQueue.end())
                {
                    m_enviroStaticQueue.erase(entityIterator);
                }
                break;
            case Material::RenderQueue::RQ_ENVIRO_MAPPING_DYNAMIC:
                entityIterator = std::find(m_enviroDynamicQueue.begin(), m_enviroDynamicQueue.end(), entity);
                if (entityIterator != m_enviroDynamicQueue.end())
                {
                    m_enviroDynamicQueue.erase(entityIterator);
                }
                break;
        }
    }
}
