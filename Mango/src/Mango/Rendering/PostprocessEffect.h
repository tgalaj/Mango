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

        virtual void init(const std::string & filterName, const std::string & fragmentShaderFilename);
        
        virtual void bind() const;
        virtual void render() const;

    protected:
        ref<Shader> m_postprocess;
        GLuint m_dummyVao;
    };
}
