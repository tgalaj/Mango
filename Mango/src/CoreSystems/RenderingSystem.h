#pragma once
#include <entityx/System.h>

#include "CoreComponents/CameraComponent.h"
#include "CoreComponents/ModelRendererComponent.h"
#include "CoreComponents/TransformComponent.h"
#include "Rendering/BloomPS.h"
#include "Rendering/DeferredRendering.h"
#include "Rendering/PostprocessEffect.h"
#include "Rendering/RenderTarget.h"
#include "Rendering/Shader.h"
#include "Rendering/Skybox.h"
#include "Rendering/SSAO.h"

namespace mango
{
    class RenderingSystem : public entityx::System<RenderingSystem>, public entityx::Receiver<RenderingSystem>
    {
    public:
        RenderingSystem();
        ~RenderingSystem();

        void configure(entityx::EntityManager& entities, entityx::EventManager& events) override;
        void update   (entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt) override;

        void receive(const entityx::ComponentAddedEvent<CameraComponent>          & event);
        void receive(const entityx::ComponentAddedEvent<ModelRendererComponent>   & event);
        void receive(const entityx::ComponentRemovedEvent<ModelRendererComponent> & event);

        void setSkybox(const std::shared_ptr<Skybox> & skybox);
        void resize(unsigned width, unsigned height);

        entityx::ComponentHandle<TransformComponent> getCameraTransform();
        entityx::ComponentHandle<CameraComponent>    getCamera();

    public:
        glm::vec3 sceneAmbientColor{};

        static bool         DEBUG_RENDERING;
        static unsigned int DEBUG_WINDOW_WIDTH;

    private:
        enum TextureMaps { SHADOW_MAP = 5 }; //TODO: move to Material class

        std::vector<entityx::Entity> m_opaqueQueue;
        std::vector<entityx::Entity> m_alphaQueue;
        std::vector<entityx::Entity> m_enviroStaticQueue;
        std::vector<entityx::Entity> m_enviroDynamicQueue;

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

        entityx::Entity m_mainCamera;

    private:
        static void initRenderingStates();

        static void beginForwardRendering();
        static void endForwardRendering  ();

        void bindMainRenderTarget();

        void applyPostprocess(std::shared_ptr<PostprocessEffect> & effect, std::shared_ptr<RenderTarget> * src, std::shared_ptr<RenderTarget> * dst);

        void renderForward                 (entityx::EntityManager& entities);
        void renderDeferred                (entityx::EntityManager& entities);
        void renderDebug                   ();
        void renderDebugLightsBoundingBoxes(entityx::EntityManager& entities);

        void renderOpaque              (const std::shared_ptr<Shader> & shader);
        void renderAlpha               (const std::shared_ptr<Shader>& shader);
        void renderEnviroMappingStatic (const std::shared_ptr<Shader>& shader);
        void renderDynamicEnviroMapping(const std::shared_ptr<Shader>& shader);
        void renderLightsForward       (entityx::EntityManager& entities);
        void renderLightsDeferred      (entityx::EntityManager& entities);

        void sortAlpha();
    };
}
