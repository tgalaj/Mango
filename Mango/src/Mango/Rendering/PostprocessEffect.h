#pragma once
#include <memory>
#include "Shader.h"

namespace mango
{
    class PostprocessEffect
    {
    public:
        PostprocessEffect();
        virtual ~PostprocessEffect();

        virtual void init(const std::string & filterName, const std::filesystem::path & fragmentShaderFilepath);
        
        virtual void bind() const;
        virtual void render() const;

    protected:
        std::shared_ptr<Shader> m_postprocess;
        GLuint m_dummyVao;
    };
}
