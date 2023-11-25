#include "mgpch.h"

#include "PostprocessEffect.h"
#include "Mango/Core/AssetManager.h"

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
        MG_PROFILE_ZONE_SCOPED;

        m_postprocess = AssetManager::createShader(filterName, "FSQ.vert", fragmentShaderFilepath);
        m_postprocess->link();
    }

    void PostprocessEffect::bind() const
    {
        MG_PROFILE_ZONE_SCOPED;

        m_postprocess->bind();
    }

    void PostprocessEffect::render() const
    {
        MG_PROFILE_ZONE_SCOPED;
        MG_PROFILE_GL_ZONE("PostprocessEffect::render");

        glBindVertexArray(m_dummyVao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
}
