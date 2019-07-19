#pragma once
#include <entityx/System.h>

#include "core_components/CameraComponent.h"
#include "core_components/TransformComponent.h"
#include "core_components/ModelRendererComponent.h"
#include "framework/rendering/Shader.h"
#include "framework/rendering/PostprocessEffect.h"
#include "framework/rendering/RenderTarget.h"
#include "framework/rendering/Skybox.h"
#include "framework/rendering/DeferredRendering.h"
#include "framework/rendering/BloomPS.h"
#include "framework/rendering/SSAO.h"

namespace Vertex
{
    class RenderingSystem : public entityx::System<RenderingSystem>, public entityx::Receiver<RenderingSystem>
    {
    public:
        RenderingSystem();
        ~RenderingSystem();

        void configure(entityx::EntityManager& entities, entityx::EventManager& events) override;
        void update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt) override;

        void receive(const entityx::ComponentAddedEvent<CameraComponent> & event);
        void receive(const entityx::ComponentAddedEvent<ModelRendererComponent>& event);
        void receive(const entityx::ComponentRemovedEvent<ModelRendererComponent>& event);

        void setSkybox(const std::shared_ptr<Skybox> & skybox);
        void resize(unsigned width, unsigned height);

        entityx::ComponentHandle<TransformComponent> getCameraTransform();
        entityx::ComponentHandle<CameraComponent> getCamera();

        glm::vec3 m_scene_ambient_color;

        static bool M_DEBUG_RENDERING;
        static unsigned int M_DEBUG_WINDOW_WIDTH;

    private:
        enum TextureMaps { SHADOW_MAP = 5 }; //TODO: move to Material class

        std::vector<entityx::Entity> m_opaque_queue;
        std::vector<entityx::Entity> m_alpha_queue;

        std::shared_ptr<Shader> m_forward_ambient;
        std::shared_ptr<Shader> m_forward_directional;
        std::shared_ptr<Shader> m_forward_point;
        std::shared_ptr<Shader> m_forward_spot;
        std::shared_ptr<Shader> m_shadow_map_generator;
        std::shared_ptr<Shader> m_omni_shadow_map_generator;
        std::shared_ptr<Shader> m_blending_shader;
        std::shared_ptr<Shader> m_debug_rendering;

        std::shared_ptr<Shader> m_gbuffer_shader;
        std::shared_ptr<Shader> m_deferred_directional;
        std::shared_ptr<Shader> m_deferred_point;
        std::shared_ptr<Shader> m_deferred_spot;

        std::shared_ptr<Shader> m_boundingbox_shader;
        std::shared_ptr<Shader> m_null_shader;
        Model m_light_bsphere;
        Model m_light_bcone;

        std::shared_ptr<PostprocessEffect> m_hdr_filter;
        std::shared_ptr<PostprocessEffect> m_fxaa_filter;
        std::shared_ptr<DeferredRendering> m_deferred_rendering;
        std::shared_ptr<BloomPS> m_bloom_filter;
        std::shared_ptr<SSAO> m_ssao_rendering;

        std::shared_ptr<RenderTarget> m_main_render_target;
        std::shared_ptr<RenderTarget> m_helper_render_target;
        std::shared_ptr<RenderTarget> m_dir_shadow_map;
        std::shared_ptr<RenderTarget> m_spot_shadow_map;
        std::shared_ptr<RenderTarget> m_omni_shadow_map;

        std::shared_ptr<Skybox> m_default_skybox;

        entityx::Entity m_main_camera;

        static void initRenderingStates();

        static void beginForwardRendering();
        static void endForwardRendering();

        void bindMainRenderTarget();

        void applyPostprocess(std::shared_ptr<PostprocessEffect> & effect, std::shared_ptr<RenderTarget> * src, std::shared_ptr<RenderTarget> * dst);

        void renderForward(entityx::EntityManager& entities);
        void renderDeferred(entityx::EntityManager& entities);
        void renderDebug();
        void renderDebugLightsBoundingBoxes(entityx::EntityManager& entities);

        void renderOpaque(const std::shared_ptr<Shader> & shader);
        void renderAlpha(const std::shared_ptr<Shader>& shader);
        void renderLightsForward(entityx::EntityManager& entities);
        void renderLightsDeferred(entityx::EntityManager& entities);

        void sortAlpha();
    };
}
