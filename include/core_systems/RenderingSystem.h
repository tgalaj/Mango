﻿#pragma once
#include <entityx/System.h>

#include "core_components/CameraComponent.h"
#include "core_components/TransformComponent.h"
#include "framework/rendering/Shader.h"
#include "framework/rendering/PostprocessEffect.h"
#include "framework/rendering/RenderTarget.h"

namespace Vertex
{
    class RenderingSystem : public entityx::System<RenderingSystem>, public entityx::Receiver<RenderingSystem>
    {
    public:
        RenderingSystem();

        void configure(entityx::EntityManager& entities, entityx::EventManager& events) override;
        void update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt) override;

        void receive(const entityx::ComponentAddedEvent<CameraComponent> & component);

        entityx::ComponentHandle<TransformComponent> getCameraTransform();
        entityx::ComponentHandle<CameraComponent> getCamera();

        glm::vec3 m_scene_ambient_color;

        static bool M_DEBUG_RENDERING;
        static float M_DEBUG_WINDOW_WIDTH;

    private:
        std::shared_ptr<Shader> m_forward_ambient;
        std::shared_ptr<Shader> m_forward_directional;
        std::shared_ptr<Shader> m_forward_point;
        std::shared_ptr<Shader> m_forward_spot;
        std::shared_ptr<Shader> m_shadow_map_generator;
        std::shared_ptr<Shader> m_omni_shadow_map_generator;

        std::shared_ptr<Shader> m_debug_rendering;

        std::shared_ptr<PostprocessEffect> m_hdr_filter;

        std::shared_ptr<RenderTarget> m_main_render_target;
        std::shared_ptr<RenderTarget> m_dir_shadow_map;
        std::shared_ptr<RenderTarget> m_spot_shadow_map;
        std::shared_ptr<RenderTarget> m_omni_shadow_map;

        entityx::Entity m_main_camera;

        static void initRenderingStates();

        static void beginForwardRendering();
        static void endForwardRendering();

        void applyPostprocess(std::shared_ptr<PostprocessEffect> & effect, std::shared_ptr<RenderTarget> * src, std::shared_ptr<RenderTarget> * dst);

        void render(entityx::EntityManager & entities);
        void renderDebug(entityx::EntityManager & entities);

        void renderAll(entityx::EntityManager & entities, const std::shared_ptr<Shader> & shader);
        void renderAllForward(entityx::EntityManager & entities, const std::shared_ptr<Shader> & shader);
        void renderLights(entityx::EntityManager & entities);
    };
}