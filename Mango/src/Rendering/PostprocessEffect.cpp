#include "mgpch.h"

#include "PostprocessEffect.h"
#include "CoreEngine/CoreAssetManager.h"

namespace mango
{
    PostprocessEffect::PostprocessEffect()
    {
        glGenVertexArrays(1, &m_dummyVao);
    }

    PostprocessEffect::~PostprocessEffect()
    {
        if(m_dummyVao != 0)
        {
            glDeleteVertexArrays(1, &m_dummyVao);
            m_dummyVao = 0;
        }
    }

    void PostprocessEffect::init(const std::string & filterName, const std::filesystem::path & fragmentShaderFilepath)
    {
        m_postprocess = CoreAssetManager::createShader(filterName, "FSQ.vert", fragmentShaderFilepath);
        m_postprocess->link();
    }

    void PostprocessEffect::bind() const
    {
        m_postprocess->bind();
    }

    void PostprocessEffect::render() const
    {
        glBindVertexArray(m_dummyVao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
}
