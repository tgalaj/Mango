﻿#pragma once
#include "Mango/Core/System.h"
#include "Mango/Events/EntityEvents.h"
#include "Mango/Events/SceneEvents.h"
#include "Mango/Rendering/Model.h"
#include "Mango/Rendering/Skybox.h"
#include "Mango/Scene/Entity.h"

namespace mango
{
    class Skybox;
    class PostprocessEffect;
    class RenderTarget;
    class Shader;
    class BloomPS;
    class SSAO;
    class DeferredRendering;
    class ModelRendererComponent;
    class TransformComponent;
    class CameraComponent;
    class Window;

    class RenderingSystem : public System
    {
    public:
        RenderingSystem();
        ~RenderingSystem() = default;

        void onInit();
        void onUpdate(float dt);
        void onDestroy();

        void receive(const ComponentAddedEvent<ModelRendererComponent>   & event);
        void receive(const ComponentRemovedEvent<ModelRendererComponent> & event);
        void receive(const ActiveSceneChangedEvent                       & event);

        void setSkybox(const std::shared_ptr<Skybox> & skybox);
        void resize(unsigned width, unsigned height);

        TransformComponent & getCameraTransform();
        CameraComponent    & getCamera();

    public:
        glm::vec3 sceneAmbientColor{};

        static bool         DEBUG_RENDERING;
        static unsigned int DEBUG_WINDOW_WIDTH;

    private:
        static void initRenderingStates();

        static void beginForwardRendering();
        static void endForwardRendering();

        void bindMainRenderTarget();

        void applyPostprocess(std::shared_ptr<PostprocessEffect>& effect, std::shared_ptr<RenderTarget>* src, std::shared_ptr<RenderTarget>* dst);

        void renderForward(Scene* scene);
        void renderDeferred(Scene* scene);
        void renderDebug();
        void renderDebugLightsBoundingBoxes(Scene* scene);

        void renderOpaque(const std::shared_ptr<Shader>& shader);
        void renderAlpha(const std::shared_ptr<Shader>& shader);
        void renderEnviroMappingStatic(const std::shared_ptr<Shader>& shader);
        void renderDynamicEnviroMapping(const std::shared_ptr<Shader>& shader);
        void renderLightsForward(Scene* scene);
        void renderLightsDeferred(Scene* scene);

        void sortAlpha();

    private:
        enum TextureMaps { SHADOW_MAP = 5 }; //TODO: move to Material class

        std::vector<Entity> m_opaqueQueue;
        std::vector<Entity> m_alphaQueue;
        std::vector<Entity> m_enviroStaticQueue;
        std::vector<Entity> m_enviroDynamicQueue;

        std::shared_ptr<Shader> m_forwardAmbient;
        std::shared_ptr<Shader> m_forwardDirectional;
        std::shared_ptr<Shader> m_forwardPoint;
        std::shared_ptr<Shader> m_forwardSpot;
        std::shared_ptr<Shader> m_shadowMapGenerator;
        std::shared_ptr<Shader> m_omniShadowMapGenerator;
        std::shared_ptr<Shader> m_blendingShader;
        std::shared_ptr<Shader> m_enviroMappingShader;
        std::shared_ptr<Shader> m_debugRendering;

        std::shared_ptr<Shader> m_gbufferShader;
        std::shared_ptr<Shader> m_deferredDirectional;
        std::shared_ptr<Shader> m_deferredPoint;
        std::shared_ptr<Shader> m_deferredSpot;

        std::shared_ptr<Shader> m_boundingboxShader;
        std::shared_ptr<Shader> m_nullShader;
        Model m_lightBoundingSphere;
        Model m_lightBoundingCone;

        std::shared_ptr<PostprocessEffect> m_hdrFilter;
        std::shared_ptr<PostprocessEffect> m_fxaaFilter;
        std::shared_ptr<DeferredRendering> m_deferredRendering;
        std::shared_ptr<BloomPS> m_bloomFilter;
        std::shared_ptr<SSAO> m_ssao;

        std::shared_ptr<RenderTarget> m_mainRenderTarget;
        std::shared_ptr<RenderTarget> m_helperRenderTarget;
        std::shared_ptr<RenderTarget> m_dirShadowMap;
        std::shared_ptr<RenderTarget> m_spotShadowMap;
        std::shared_ptr<RenderTarget> m_omniShadowMap;

        std::shared_ptr<Skybox> m_skybox;

        Entity m_primaryCamera;
        Scene* m_activeScene = nullptr;
        std::shared_ptr<Window> m_mainWindow;
    };
}
