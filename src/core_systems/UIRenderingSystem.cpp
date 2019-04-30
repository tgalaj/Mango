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
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

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

        // Iterate through all characters
        std::string text = text_component->m_text;
        std::string::const_iterator c;

        float x = text_component->m_position.x;
        float y = text_component->m_position.y;

        for (c = text.begin(); c != text.end(); c++)
        {
            Font::Character ch = text_component->m_font->getCharacter(*c);

            GLfloat xpos = x + ch.m_bearing.x * text_component->m_scale;
            GLfloat ypos = y - (ch.m_size.y - ch.m_bearing.y) * text_component->m_scale;

            GLfloat w = ch.m_size.x * text_component->m_scale;
            GLfloat h = ch.m_size.y * text_component->m_scale;
            // Update VBO for each character
            GLfloat vertices[6][4] = {
                { xpos,     ypos + h,   0.0, 0.0 },            
                { xpos,     ypos,       0.0, 1.0 },
                { xpos + w, ypos,       1.0, 1.0 },

                { xpos,     ypos + h,   0.0, 0.0 },
                { xpos + w, ypos,       1.0, 1.0 },
                { xpos + w, ypos + h,   1.01, 0.0 }           
            };
            // Render glyph texture over quad
            glBindTextureUnit(0, ch.m_texture_id);
            // Update content of VBO memory
            glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 
            // Render quad
            glDrawArrays(GL_TRIANGLES, 0, 6);
            // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
            x += (ch.m_advance >> 6) * text_component->m_scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
        }
    }
}

