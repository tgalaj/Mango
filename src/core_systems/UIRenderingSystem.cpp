#include "core_systems/UIRenderingSystem.h"

#include "core_engine/CoreAssetManager.h"
#include "core_engine/CoreServices.h"
#include "framework/window/Window.h"
#include "core_components/UITextComponent.h"

namespace Vertex
{
    UIRenderingSystem::UIRenderingSystem() {}
    
    UIRenderingSystem::~UIRenderingSystem()
    {
        glDeleteBuffers(1, &m_vbo);
        glDeleteVertexArrays(1, &m_vao);
    }

    void UIRenderingSystem::configure(entityx::EntityManager & entities, entityx::EventManager & events)
    {
        init();

        m_ui_shader = CoreAssetManager::createShader("UI", "UI.vert", "UI.frag");
        m_ui_shader->link();

        m_projection = glm::ortho(0.0f, float(Window::getWidth()), 0.0f, float(Window::getHeight()), -1.0f, 1.0f);
    }

    void UIRenderingSystem::update(entityx::EntityManager & entities, entityx::EventManager & events, entityx::TimeDelta dt)
    {
        prepareRendering();

        // TODO: sort text components based on font - bind texture atlas before text rendering
        entityx::ComponentHandle<UITextComponent> text_component;

        for(auto entity : entities.entities_with_components(text_component))
        {
            renderText(text_component);
        }
    }

    void UIRenderingSystem::init()
    {
        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);

        glGenBuffers(1, &m_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    }

    void UIRenderingSystem::prepareRendering()
    {
        glClear(GL_DEPTH_BUFFER_BIT);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glBindVertexArray(m_vao);

        m_ui_shader->bind();
        m_ui_shader->setUniform("projection", m_projection);
    }

    void UIRenderingSystem::renderText(entityx::ComponentHandle<UITextComponent>& text_component)
    {
        m_ui_shader->setUniform("text_color", text_component->m_color);

        std::string text = text_component->m_text;
        glm::vec2 font_atlas_size = text_component->m_font->getFontAtlasDimensions();

        struct vertex
        {
            GLfloat x;
            GLfloat y;
            GLfloat u;
            GLfloat v;
        };
        std::vector<vertex> vertices;

        float x = text_component->m_position.x;
        float y = text_component->m_position.y;


        /* Iterate through all characters */
        for (auto c = text.begin(); c != text.end(); ++c)
        {
            Font::Character ch = text_component->m_font->getCharacter(*c);

            GLfloat xpos =  x + ch.m_bearing.x * text_component->m_scale;
            GLfloat ypos = -y - ch.m_bearing.y * text_component->m_scale;

            GLfloat w = ch.m_size.x * text_component->m_scale;
            GLfloat h = ch.m_size.y * text_component->m_scale;

            x += ch.m_advance.x * text_component->m_scale;
            y += ch.m_advance.y * text_component->m_scale;

            /* Skip glyphs that have no pixels */
            if(!int(w) || !int(h))
            {
                continue;
            }

            vertices.push_back({ xpos,     -ypos,     ch.m_glyph_texture_offset.x,                                     ch.m_glyph_texture_offset.y });
            vertices.push_back({ xpos,     -ypos - h, ch.m_glyph_texture_offset.x,                                     ch.m_glyph_texture_offset.y + ch.m_size.y / font_atlas_size.y });
            vertices.push_back({ xpos + w, -ypos,     ch.m_glyph_texture_offset.x + (ch.m_size.x / font_atlas_size.x), ch.m_glyph_texture_offset.y });
            vertices.push_back({ xpos + w, -ypos,     ch.m_glyph_texture_offset.x + (ch.m_size.x / font_atlas_size.x), ch.m_glyph_texture_offset.y });
            vertices.push_back({ xpos,     -ypos - h, ch.m_glyph_texture_offset.x,                                     ch.m_glyph_texture_offset.y + ch.m_size.y / font_atlas_size.y });
            vertices.push_back({ xpos + w, -ypos - h, ch.m_glyph_texture_offset.x + (ch.m_size.x / font_atlas_size.x), ch.m_glyph_texture_offset.y + ch.m_size.y / font_atlas_size.y });
        }

        // TODO: VAO and VBO should be inside text_component (if text component gets changed, it should update vertex data itself)
        // TODO: Add EBO
        text_component->m_font->bindFontTexture();
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), vertices.data(), GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    }
}

