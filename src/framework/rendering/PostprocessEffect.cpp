#include "framework/rendering/PostprocessEffect.h"
#include "core_engine/CoreAssetManager.h"

namespace Vertex
{
    PostprocessEffect::PostprocessEffect()
    {
        glGenVertexArrays(1, &m_dummy_vao_id);
    }

    PostprocessEffect::~PostprocessEffect()
    {
        if(m_dummy_vao_id != 0)
        {
            glDeleteVertexArrays(1, &m_dummy_vao_id);
            m_dummy_vao_id = 0;
        }
    }

    void PostprocessEffect::init(const std::string & filter_name, const std::string & fragment_shader_path)
    {
        m_postprocess = CoreAssetManager::createShader(filter_name, "FSQ.vert", fragment_shader_path);
        m_postprocess->link();
    }

    void PostprocessEffect::bind() const
    {
        m_postprocess->bind();
    }

    void PostprocessEffect::render() const
    {
        glBindVertexArray(m_dummy_vao_id);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
}
