#pragma once
#include <entityx/System.h>

#include "core_components/TransformComponent.h"
#include "core_components/ModelRendererComponent.h"
#include "framework/rendering/Shader.h"
#include "core_components/UITextComponent.h"

namespace Vertex
{
    class UIRenderingSystem : public entityx::System<UIRenderingSystem>, public entityx::Receiver<UIRenderingSystem>
    {
    public:
        UIRenderingSystem();
        ~UIRenderingSystem();

        void configure(entityx::EntityManager& entities, entityx::EventManager& events) override;
        void update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt) override;

    private:
        void init();

        std::shared_ptr<Shader> m_ui_shader;
        glm::mat4 m_projection;

        GLuint m_vao, m_vbo;

        void prepareRendering();
        void renderText(entityx::ComponentHandle<UITextComponent> & text_component);
    };
}
