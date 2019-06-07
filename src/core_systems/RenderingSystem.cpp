#include <core_systems/RenderingSystem.h>

#include "core_engine/CoreAssetManager.h"
#include "core_engine/CoreServices.h"
#include "framework/window/Window.h"
#include "core_components/DirectionalLightComponent.h"
#include "core_components/PointLightComponent.h"
#include "core_components/SpotLightComponent.h"
#include "framework/utilities/ShaderGlobals.h"

namespace Vertex
{
    bool         RenderingSystem::M_DEBUG_RENDERING    = false;
    unsigned int RenderingSystem::M_DEBUG_WINDOW_WIDTH = 0;

    RenderingSystem::RenderingSystem() {}
    
    RenderingSystem::~RenderingSystem() 
    {
        m_opaque_queue.clear();
        m_alpha_queue.clear();
    }

    void RenderingSystem::configure(entityx::EntityManager & entities, entityx::EventManager & events)
    {
        events.subscribe<entityx::ComponentAddedEvent<CameraComponent>>(*this);
        events.subscribe<entityx::ComponentAddedEvent<ModelRendererComponent>>(*this);
        events.subscribe<entityx::ComponentRemovedEvent<ModelRendererComponent>>(*this);

        CoreAssetManager::createTexture2D1x1("default_white",  glm::uvec4(255, 255, 255, 255));
        CoreAssetManager::createTexture2D1x1("default_black",  glm::uvec4(0,   0,   0,   255));
        CoreAssetManager::createTexture2D1x1("default_normal", glm::uvec4(128, 127, 254, 255));

        m_opaque_queue.reserve(50);
        m_alpha_queue.reserve(5);

        m_forward_ambient = CoreAssetManager::createShader("Forward-Ambient", "Forward-Light.vert", "Forward-Ambient.frag");
        m_forward_ambient->link();

        m_forward_directional = CoreAssetManager::createShader("Forward-Directional", "Forward-Light.vert", "Forward-Directional.frag");
        m_forward_directional->link();

        m_forward_point = CoreAssetManager::createShader("Forward-Point", "Forward-Light.vert", "Forward-Point.frag");
        m_forward_point->link();

        m_forward_spot = CoreAssetManager::createShader("Forward-Spot", "Forward-Light.vert", "Forward-Spot.frag");
        m_forward_spot->link();

        m_debug_rendering = CoreAssetManager::createShader("Debug-Rendering", "FSQ.vert", "DebugRendering.frag");
        m_debug_rendering->link();

        m_shadow_map_generator = CoreAssetManager::createShader("Shadow-Map-Gen", "Shadow-Map-Gen.vert", "Shadow-Map-Gen.frag");
        m_shadow_map_generator->link();

        m_omni_shadow_map_generator = CoreAssetManager::createShader("Omni-Shadow-Map-Gen", "Omni-Shadow-Map-Gen.vert", "Omni-Shadow-Map-Gen.frag", "Omni-Shadow-Map-Gen.geom");
        m_omni_shadow_map_generator->link();

        m_blending_shader = CoreAssetManager::createShader("Blending-Shader", "Blending.vert", "Blending.frag");
        m_blending_shader->link();

        m_gbuffer_shader = CoreAssetManager::createShader("GBuffer", "GBuffer.vert", "GBuffer.frag");
        m_gbuffer_shader->link();

        m_deferred_directional = CoreAssetManager::createShader("Deferred-Directional", "FSQ.vert", "Deferred-Directional.frag");
        m_deferred_directional->link();

        M_DEBUG_WINDOW_WIDTH = GLuint(Window::getWidth() / 5.0f);
        m_scene_ambient_color = glm::vec3(0.18f);

        m_hdr_filter = std::make_shared<PostprocessEffect>();
        m_hdr_filter->init("PS-HDR", "PS-HDR.frag");

        m_deferred_rendering = std::make_shared<DeferredRendering>();
        m_deferred_rendering->init();
        m_deferred_rendering->createGBuffer();

        m_main_render_target = std::make_shared<RenderTarget>();
        m_main_render_target->create(Window::getWidth(), Window::getHeight(), RenderTarget::ColorInternalFormat::RGBA16F, RenderTarget::DepthInternalFormat::DEPTH24);

        m_dir_shadow_map = std::make_shared<RenderTarget>();
        m_dir_shadow_map->create(2048, 2048, RenderTarget::DepthInternalFormat::DEPTH24);
         
        m_spot_shadow_map = std::make_shared<RenderTarget>();
        m_spot_shadow_map->create(1024, 1024, RenderTarget::DepthInternalFormat::DEPTH24);

        m_omni_shadow_map = std::make_shared<RenderTarget>();
        m_omni_shadow_map->create(512, 512, RenderTarget::DepthInternalFormat::DEPTH24, RenderTarget::RenderTargetType::TexCube);

        initRenderingStates();
    }

    void RenderingSystem::update(entityx::EntityManager & entities, entityx::EventManager & events, entityx::TimeDelta dt)
    {
        render(entities);

        if (M_DEBUG_RENDERING)
        {
            renderDebug();
        }
    }

    void RenderingSystem::receive(const entityx::ComponentAddedEvent<CameraComponent>& event)
    {
        if (!m_main_camera)
        {
            m_main_camera = event.entity;
        }
    }

    void RenderingSystem::receive(const entityx::ComponentAddedEvent<ModelRendererComponent>& event)
    {
        switch (event.component->getRenderQueue())
        {
        case ModelRendererComponent::RenderQueue::RQ_OPAQUE:
            m_opaque_queue.push_back(event.entity);
            break;
        case ModelRendererComponent::RenderQueue::RQ_ALPHA:
            m_alpha_queue.push_back(event.entity);
            break;
        }
    }

    void RenderingSystem::receive(const entityx::ComponentRemovedEvent<ModelRendererComponent>& event)
    {
        std::vector<entityx::Entity>::iterator entity_it;

        switch (event.component->getRenderQueue())
        {
        case ModelRendererComponent::RenderQueue::RQ_OPAQUE:
            entity_it = std::find(m_opaque_queue.begin(), m_opaque_queue.end(), event.entity);
            if (entity_it != m_opaque_queue.end())
            {
                m_opaque_queue.erase(entity_it);
            }
            break;
        case ModelRendererComponent::RenderQueue::RQ_ALPHA:
            entity_it = std::find(m_alpha_queue.begin(), m_alpha_queue.end(), event.entity);
            if (entity_it != m_alpha_queue.end())
            {
                m_alpha_queue.erase(entity_it);
            }
            break;
        }
    }

    void RenderingSystem::setSkybox(const std::shared_ptr<Skybox>& skybox)
    {
        m_default_skybox = skybox;
    }

    void RenderingSystem::resize(unsigned width, unsigned height)
    {
        m_main_render_target->clear();
        m_main_render_target->create(width, height, RenderTarget::ColorInternalFormat::RGBA16F, RenderTarget::DepthInternalFormat::DEPTH24);
    }

    entityx::ComponentHandle<TransformComponent> RenderingSystem::getCameraTransform()
    {
        return m_main_camera.component<TransformComponent>();
    }

    entityx::ComponentHandle<CameraComponent> RenderingSystem::getCamera()
    {
        return m_main_camera.component<CameraComponent>();
    }

    void RenderingSystem::initRenderingStates()
    {
        glFrontFace(GL_CCW);
        glCullFace(GL_BACK);

        glEnable(GL_CULL_FACE);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);

        glClearColor(0, 0, 0, 1);
    }

    void RenderingSystem::beginForwardRendering()
    {
        glBlendFunc(GL_ONE, GL_ONE);
        glDepthMask(GL_FALSE);
        glDepthFunc(GL_EQUAL);

    }

    void RenderingSystem::endForwardRendering()
    {
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
    }

    void RenderingSystem::beginDeferredRendering()
    {
        glDisable(GL_DEPTH_TEST);
        glBlendFunc(GL_ONE, GL_ONE);
    }

    void RenderingSystem::endDeferredRendering()
    {
        glEnable(GL_DEPTH_TEST);
    }

    void RenderingSystem::bindMainRenderTarget()
    {
        m_main_render_target->bind();
    }

    void RenderingSystem::applyPostprocess(std::shared_ptr<PostprocessEffect> & effect, 
                                           std::shared_ptr<RenderTarget>      * src, 
                                           std::shared_ptr<RenderTarget>      * dst)
    {
        if(dst == nullptr)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, Window::getWidth(), Window::getHeight());
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

    void RenderingSystem::render(entityx::EntityManager& entities)
    {
        /* Render data to GBuffer */
        glDisable(GL_BLEND);

        m_deferred_rendering->bindGBuffer(); 
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_gbuffer_shader->bind();
        renderOpaque(m_gbuffer_shader);

        /* Render everything to offscreen FBO */
        //m_main_render_target->bind();
        //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* Render deferred shading */
        //m_deferred_rendering->bind();
        //m_deferred_rendering->bindGBufferTextures();
        //m_deferred_rendering->render();



        /* Render lights to light buffer */
        glEnable(GL_BLEND);
        renderLightsDeferred(entities);

        //m_forward_ambient->bind();
        //m_forward_ambient->setUniform("s_scene_ambient", m_scene_ambient_color);
        //renderOpaque(m_forward_ambient); 
        //renderLightsForward(entities);

        m_main_render_target->bind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* Sort transparent objects back to front */
        sortAlpha();

        /* Render transparent objects */
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_CULL_FACE);
        m_blending_shader->bind();
        renderAlpha(m_blending_shader);
        glEnable(GL_CULL_FACE);

        /* Render skybox */
        if (m_default_skybox != nullptr)
        {
            m_default_skybox->render(getCamera()->m_projection, getCamera()->m_view);
        }

        /* Apply postprocess effect */
        applyPostprocess(m_hdr_filter, &m_main_render_target, 0);
    }

    void RenderingSystem::renderDebug()
    {
        glDisable(GL_BLEND);
        glClear(GL_DEPTH_BUFFER_BIT);

        m_debug_rendering->bind();

        m_debug_rendering->setSubroutine(Shader::Type::FRAGMENT, "debugColorTarget");
        m_deferred_rendering->bindGBufferTexture(0, (GLuint)DeferredRendering::GBufferPropertyName::POSITION);
        glViewport(M_DEBUG_WINDOW_WIDTH * 0, 0, M_DEBUG_WINDOW_WIDTH, M_DEBUG_WINDOW_WIDTH / Window::getAspectRatio());
        m_deferred_rendering->render();

        m_deferred_rendering->bindGBufferTexture(0, (GLuint)DeferredRendering::GBufferPropertyName::ALBEDO_SPECULAR);
        glViewport(M_DEBUG_WINDOW_WIDTH * 1, 0, M_DEBUG_WINDOW_WIDTH, M_DEBUG_WINDOW_WIDTH / Window::getAspectRatio());
        m_deferred_rendering->render();

        m_deferred_rendering->bindGBufferTexture(0, (GLuint)DeferredRendering::GBufferPropertyName::NORMAL);
        glViewport(M_DEBUG_WINDOW_WIDTH * 2, 0, M_DEBUG_WINDOW_WIDTH, M_DEBUG_WINDOW_WIDTH / Window::getAspectRatio());
        m_deferred_rendering->render();

        m_deferred_rendering->bindLightTexture();
        glViewport(M_DEBUG_WINDOW_WIDTH * 4, 0, M_DEBUG_WINDOW_WIDTH, M_DEBUG_WINDOW_WIDTH / Window::getAspectRatio());
        m_deferred_rendering->render();

        m_debug_rendering->setSubroutine(Shader::Type::FRAGMENT, "debugDepthTarget");
        m_deferred_rendering->bindGBufferTexture(0, (GLuint)DeferredRendering::GBufferPropertyName::DEPTH);
        glViewport(M_DEBUG_WINDOW_WIDTH * 3, 0, M_DEBUG_WINDOW_WIDTH, M_DEBUG_WINDOW_WIDTH / Window::getAspectRatio());
        m_deferred_rendering->render();

        glEnable(GL_BLEND);
    }

    void RenderingSystem::renderOpaque(const std::shared_ptr<Shader> & shader)
    {
        ModelRendererComponent* model_renderer;
        TransformComponent* transform;

        for (auto& entity : m_opaque_queue)
        {
            model_renderer = entity.component<ModelRendererComponent>().get();
            transform      = entity.component<TransformComponent>().get();

            shader->updateGlobalUniforms(*transform);
            model_renderer->m_model.render(*shader);
        }
    }

    void RenderingSystem::renderAlpha(const std::shared_ptr<Shader>& shader)
    {
        ModelRendererComponent* model_renderer;
        TransformComponent* transform;

        for (auto& entity : m_alpha_queue)
        {
            model_renderer = entity.component<ModelRendererComponent>().get();
            transform = entity.component<TransformComponent>().get();

            shader->updateGlobalUniforms(*transform);
            model_renderer->m_model.render(*shader);
        }
    }

    void RenderingSystem::renderLightsForward(entityx::EntityManager& entities)
    {
        /*
         * TODO:
         * Transform shadow info into light component;
         * Sort lights based on the parameter if light casts shadows or not;
         * Then render lights with shadows and then lights without shadows;
         */

        entityx::ComponentHandle<DirectionalLightComponent> directional_light;
        entityx::ComponentHandle<PointLightComponent>       point_light;
        entityx::ComponentHandle<SpotLightComponent>        spot_light;
        entityx::ComponentHandle<TransformComponent>        transform;

        for(auto entity : entities.entities_with_components(directional_light, transform))
        {
            ShadowInfo shadow_info = directional_light->getShadowInfo();
            glm::mat4 light_matrix = glm::mat4(1.0f);

            if(shadow_info.getCastsShadows())
            {
                m_shadow_map_generator->bind();

                m_dir_shadow_map->bind();
                glClear(GL_DEPTH_BUFFER_BIT);

                light_matrix = shadow_info.getProjection() * glm::lookAt(-transform->direction(), glm::vec3(0.0f), glm::vec3(0, 1, 0));
                m_shadow_map_generator->setUniform("s_light_matrix", light_matrix);

                glCullFace(GL_FRONT);
                renderOpaque(m_shadow_map_generator);
                glCullFace(GL_BACK);
            }

            bindMainRenderTarget();
            
            m_forward_directional->bind();
            m_dir_shadow_map->bindTexture(SHADOW_MAP);

            m_forward_directional->setUniform(S_DIRECTIONAL_LIGHT ".base.color",     directional_light->m_color);
            m_forward_directional->setUniform(S_DIRECTIONAL_LIGHT ".base.intensity", directional_light->m_intensity);
            m_forward_directional->setUniform(S_DIRECTIONAL_LIGHT ".direction",      transform->direction());
            m_forward_directional->setUniform("s_light_matrix", light_matrix);

            beginForwardRendering();
            renderOpaque(m_forward_directional);
            endForwardRendering();
        }

        for (auto entity : entities.entities_with_components(point_light, transform))
        {
            ShadowInfo shadow_info = point_light->getShadowInfo();
            glm::mat4 light_matrices[6];

            if (shadow_info.getCastsShadows())
            {
                m_omni_shadow_map_generator->bind();

                m_omni_shadow_map->bind();
                glClear(GL_DEPTH_BUFFER_BIT);

                light_matrices[0] = shadow_info.getProjection() * glm::lookAt(transform->position(), transform->position() + glm::vec3( 1,  0,  0), glm::vec3(0, -1,  0));
                light_matrices[1] = shadow_info.getProjection() * glm::lookAt(transform->position(), transform->position() + glm::vec3(-1,  0,  0), glm::vec3(0, -1,  0));
                light_matrices[2] = shadow_info.getProjection() * glm::lookAt(transform->position(), transform->position() + glm::vec3( 0,  1,  0), glm::vec3(0,  0,  1));
                light_matrices[3] = shadow_info.getProjection() * glm::lookAt(transform->position(), transform->position() + glm::vec3( 0, -1,  0), glm::vec3(0,  0, -1));
                light_matrices[4] = shadow_info.getProjection() * glm::lookAt(transform->position(), transform->position() + glm::vec3( 0,  0,  1), glm::vec3(0, -1,  0));
                light_matrices[5] = shadow_info.getProjection() * glm::lookAt(transform->position(), transform->position() + glm::vec3( 0,  0, -1), glm::vec3(0, -1,  0));

                m_omni_shadow_map_generator->setUniform("s_light_matrices", light_matrices, 6);
                m_omni_shadow_map_generator->setUniform("s_light_pos", transform->position());
                m_omni_shadow_map_generator->setUniform("s_far_plane", 100.0f);

                glCullFace(GL_FRONT);
                renderOpaque(m_omni_shadow_map_generator);
                glCullFace(GL_BACK);
            }

            bindMainRenderTarget();

            m_forward_point->bind();
            m_omni_shadow_map->bindTexture(SHADOW_MAP);

            m_forward_point->setUniform(S_POINT_LIGHT ".base.color",      point_light->m_color);
            m_forward_point->setUniform(S_POINT_LIGHT ".base.intensity",  point_light->m_intensity);
            m_forward_point->setUniform(S_POINT_LIGHT ".atten.constant",  point_light->m_attenuation.m_constant);
            m_forward_point->setUniform(S_POINT_LIGHT ".atten.linear",    point_light->m_attenuation.m_linear);
            m_forward_point->setUniform(S_POINT_LIGHT ".atten.quadratic", point_light->m_attenuation.m_quadratic);
            m_forward_point->setUniform(S_POINT_LIGHT ".position",        transform->position());
            m_forward_point->setUniform(S_POINT_LIGHT ".range",           point_light->m_range);
            m_forward_point->setUniform("s_far_plane", 100.0f);

            beginForwardRendering();
            renderOpaque(m_forward_point);
            endForwardRendering();
        }

        for (auto entity : entities.entities_with_components(spot_light, transform))
        {
            ShadowInfo shadow_info = spot_light->getShadowInfo();
            glm::mat4 light_matrix = glm::mat4(1.0f);

            if (shadow_info.getCastsShadows())
            {
                m_shadow_map_generator->bind();

                m_spot_shadow_map->bind();
                glClear(GL_DEPTH_BUFFER_BIT);

                light_matrix = shadow_info.getProjection() * glm::lookAt(transform->position(), transform->position() + transform->direction(), glm::vec3(0, 1, 0));
                m_shadow_map_generator->setUniform("s_light_matrix", light_matrix);

                glCullFace(GL_FRONT);
                renderOpaque(m_shadow_map_generator);
                glCullFace(GL_BACK);
            }

            bindMainRenderTarget();

            m_forward_spot->bind();
            m_spot_shadow_map->bindTexture(SHADOW_MAP);

            m_forward_spot->setUniform(S_SPOT_LIGHT ".point.base.color",      spot_light->m_color);
            m_forward_spot->setUniform(S_SPOT_LIGHT ".point.base.intensity",  spot_light->m_intensity);
            m_forward_spot->setUniform(S_SPOT_LIGHT ".point.atten.constant",  spot_light->m_attenuation.m_constant);
            m_forward_spot->setUniform(S_SPOT_LIGHT ".point.atten.linear",    spot_light->m_attenuation.m_linear);
            m_forward_spot->setUniform(S_SPOT_LIGHT ".point.atten.quadratic", spot_light->m_attenuation.m_quadratic);
            m_forward_spot->setUniform(S_SPOT_LIGHT ".point.position",        transform->position());
            m_forward_spot->setUniform(S_SPOT_LIGHT ".point.range",           spot_light->m_range);
            m_forward_spot->setUniform(S_SPOT_LIGHT ".direction",             transform->direction());
            m_forward_spot->setUniform(S_SPOT_LIGHT ".cutoff",                spot_light->getCutOffAngle());
            m_forward_spot->setUniform("s_light_matrix", light_matrix);

            beginForwardRendering();
            renderOpaque(m_forward_spot);
            endForwardRendering();
        }
    }

    void RenderingSystem::renderLightsDeferred(entityx::EntityManager& entities)
    {
        entityx::ComponentHandle<DirectionalLightComponent> directional_light;
        entityx::ComponentHandle<PointLightComponent>       point_light;
        entityx::ComponentHandle<SpotLightComponent>        spot_light;
        entityx::ComponentHandle<TransformComponent>        transform;


        m_deferred_rendering->bindLightBuffer();
        glClear(GL_COLOR_BUFFER_BIT);

        /* Deferred Directional Lights */
        for(auto entity : entities.entities_with_components(directional_light, transform))
        {
//            ShadowInfo shadow_info = directional_light->getShadowInfo();
//            glm::mat4 light_matrix = glm::mat4(1.0f);
//
//            if(shadow_info.getCastsShadows())
//            {
//                m_shadow_map_generator->bind();
//
//                m_dir_shadow_map->bind();
//                glClear(GL_DEPTH_BUFFER_BIT);
//
//                light_matrix = shadow_info.getProjection() * glm::lookAt(-transform->direction(), glm::vec3(0.0f), glm::vec3(0, 1, 0));
//                m_shadow_map_generator->setUniform("s_light_matrix", light_matrix);
//
//                glCullFace(GL_FRONT);
//                renderOpaque(m_shadow_map_generator);
//                glCullFace(GL_BACK);
//            }

            m_deferred_directional->bind();
            m_deferred_rendering->bindLightBuffer();
            m_deferred_rendering->bindGBufferTextures();
            //m_dir_shadow_map->bindTexture(SHADOW_MAP);

            m_deferred_directional->setUniform(S_DIRECTIONAL_LIGHT ".base.color",     directional_light->m_color);
            m_deferred_directional->setUniform(S_DIRECTIONAL_LIGHT ".base.intensity", directional_light->m_intensity);
            m_deferred_directional->setUniform(S_DIRECTIONAL_LIGHT ".direction",      transform->direction());
            //m_deferred_directional->setUniform("s_light_matrix", light_matrix);

            beginDeferredRendering();
            m_deferred_rendering->render();
            endDeferredRendering();
        }
    }

    void RenderingSystem::sortAlpha()
    {
        auto cam_pos = getCameraTransform()->position();

        std::sort(m_alpha_queue.begin(), m_alpha_queue.end(), 
                   [&cam_pos](entityx::Entity & obj1, entityx::Entity & obj2)
                         {
                            const auto pos1 = obj1.component<TransformComponent>()->position();
                            const auto pos2 = obj2.component<TransformComponent>()->position();

                            return glm::length(cam_pos - pos1) >= glm::length(cam_pos - pos2);
                         });
    }
}
